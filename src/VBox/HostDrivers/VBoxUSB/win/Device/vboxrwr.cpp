/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    vboxrwr.cpp

Abstract:

    This file has routines to perform reads and writes.
    The read and writes are for bulk transfers.

Environment:

    Kernel mode

Notes:

    Copyright (c) 2000 Microsoft Corporation.
    All Rights Reserved.

--*/

#include "vboxusb.h"
#include "vboxpnp.h"
#include "vboxpwr.h"
#include "vboxdev.h"
#include "vboxrwr.h"
#include <VBox/log.h>
#include <iprt/assert.h>


PVBOXUSB_PIPE_CONTEXT
VBoxUSB_PipeWithName(
    IN PDEVICE_OBJECT  DeviceObject,
    IN PUNICODE_STRING FileName
    )
/*++

Routine Description:

    This routine will pass the string pipe name and
    fetch the pipe number.

Arguments:

    DeviceObject - pointer to DeviceObject
    FileName - string pipe name

Return Value:

    The device extension maintains a pipe context for
    the pipes on 82930 board.
    This routine returns the pointer to this context in
    the device extension for the "FileName" pipe.

--*/
{
    LONG                  ix;
    ULONG                 uval;
    ULONG                 nameLength;
    ULONG                 umultiplier;
    PDEVICE_EXTENSION     deviceExtension;
    PVBOXUSB_PIPE_CONTEXT pipeContext;

    //
    // initialize variables
    //
    pipeContext = NULL;
    //
    // typedef WCHAR *PWSTR;
    //
    nameLength = (FileName->Length / sizeof(WCHAR));
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    dprintf(("VBoxUSB_PipeWithName - begins\n"));

    if(nameLength != 0) {

        dprintf(("Filename = %ws nameLength = %d\n", FileName->Buffer, nameLength));

        //
        // Parse the pipe#
        //
        ix = nameLength - 1;

        // if last char isn't digit, decrement it.
        while((ix > -1) &&
              ((FileName->Buffer[ix] < (WCHAR) '0')  ||
               (FileName->Buffer[ix] > (WCHAR) '9')))             {

            ix--;
        }

        if(ix > -1) {

            uval = 0;
            umultiplier = 1;

            // traversing least to most significant digits.

            while((ix > -1) &&
                  (FileName->Buffer[ix] >= (WCHAR) '0') &&
                  (FileName->Buffer[ix] <= (WCHAR) '9'))          {

                uval += (umultiplier *
                         (ULONG) (FileName->Buffer[ix] - (WCHAR) '0'));

                ix--;
                umultiplier *= 10;
            }

            if(uval < 6 && deviceExtension->PipeContext) {

                pipeContext = &deviceExtension->PipeContext[uval];
            }
        }
    }

    dprintf(("VBoxUSB_PipeWithName - ends\n"));

    return pipeContext;
}

NTSTATUS
VBoxUSB_DispatchReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{
	/* should never be here ! since all communication is performed via IOCTL */
	AssertFailed();

    NTSTATUS Status = STATUS_ACCESS_DENIED;
    PDEVICE_EXTENSION deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    dprintf(("VBoxUSB_DispatchReadWrite - begins\n"));

    if(deviceExtension->DeviceState != Working) {
        dprintf(("Invalid device state\n"));
        Status = STATUS_INVALID_DEVICE_STATE;
    }

    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}

NTSTATUS
VBoxUSB_ReadWriteCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
/*++

Routine Description:

    This is the completion routine for reads/writes
    If the irp completes with success, we check if we
    need to recirculate this irp for another stage of
    transfer. In this case return STATUS_MORE_PROCESSING_REQUIRED.
    if the irp completes in error, free all memory allocs and
    return the status.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet
    Context - context passed to the completion routine.

Return Value:

    NT status value

--*/
{
    ULONG               stageLength;
    NTSTATUS            ntStatus;
    PIO_STACK_LOCATION  nextStack;
    PVBOXUSB_RW_CONTEXT rwContext;

    //
    // initialize variables
    //
    rwContext = (PVBOXUSB_RW_CONTEXT) Context;
    ntStatus = Irp->IoStatus.Status;

    UNREFERENCED_PARAMETER(DeviceObject);
    dprintf(("VBoxUSB_ReadWriteCompletion - begins\n"));

    //
    // successfully performed a stageLength of transfer.
    // check if we need to recirculate the irp.
    //
    if(NT_SUCCESS(ntStatus)) {

        if(rwContext) {

            rwContext->Numxfer +=
              rwContext->Urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

            if(rwContext->Length) {

                //
                // another stage transfer
                //
                dprintf(("Another stage transfer...\n"));

                if(rwContext->Length > VBOXUSB_MAX_TRANSFER_SIZE) {

                    stageLength = VBOXUSB_MAX_TRANSFER_SIZE;
                }
                else {

                    stageLength = rwContext->Length;
                }

                // the source MDL is not mapped and so when the lower driver
                // calls MmGetSystemAddressForMdl(Safe) on Urb->Mdl (target Mdl),
                // system PTEs are used.
                // IoFreeMdl calls MmPrepareMdlForReuse to release PTEs (unlock
                // VA address before freeing any Mdl
                // Rather than calling IoFreeMdl and IoAllocateMdl each time,
                // just call MmPrepareMdlForReuse
                // Not calling MmPrepareMdlForReuse will leak system PTEs
                //
                MmPrepareMdlForReuse(rwContext->Mdl);

                IoBuildPartialMdl(Irp->MdlAddress,
                                  rwContext->Mdl,
                                  (PVOID) rwContext->VirtualAddress,
                                  stageLength);

                //
                // reinitialize the urb
                //
                rwContext->Urb->UrbBulkOrInterruptTransfer.TransferBufferLength
                                                                  = stageLength;
                rwContext->VirtualAddress += stageLength;
                rwContext->Length -= stageLength;

                nextStack = IoGetNextIrpStackLocation(Irp);
                nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
                nextStack->Parameters.Others.Argument1 = rwContext->Urb;
                nextStack->Parameters.DeviceIoControl.IoControlCode =
                                            IOCTL_INTERNAL_USB_SUBMIT_URB;

                IoSetCompletionRoutine(Irp,
                                       VBoxUSB_ReadWriteCompletion,
                                       rwContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                IoCallDriver(rwContext->DeviceExtension->TopOfStackDeviceObject,
                             Irp);

                return STATUS_MORE_PROCESSING_REQUIRED;
            }
            else {

                //
                // this is the last transfer
                //

                Irp->IoStatus.Information = rwContext->Numxfer;
            }
        }
    }
    else {

        dprintf(("ReadWriteCompletion - failed with status = %X\n", ntStatus));
    }

    if(rwContext) {

        //
        // dump rwContext
        //
        dprintf(("rwContext->Urb             = %X\n",
                             rwContext->Urb));
        dprintf(("rwContext->Mdl             = %X\n",
                             rwContext->Mdl));
        dprintf(("rwContext->Length          = %d\n",
                             rwContext->Length));
        dprintf(("rwContext->Numxfer         = %d\n",
                             rwContext->Numxfer));
        dprintf(("rwContext->VirtualAddress  = %X\n",
                             rwContext->VirtualAddress));
        dprintf(("rwContext->DeviceExtension = %X\n",
                             rwContext->DeviceExtension));

        dprintf(("VBoxUSB_ReadWriteCompletion::"));
        VBoxUSB_IoDecrement(rwContext->DeviceExtension);

        ExFreePool(rwContext->Urb);
        IoFreeMdl(rwContext->Mdl);
        ExFreePool(rwContext);
    }

    dprintf(("VBoxUSB_ReadWriteCompletion - ends\n"));

    return ntStatus;
}

