; $Id: VBoxBiosAlternative.asm $ 
;; @file
; Auto Generated source file. Do not edit.
;

;
; Source file: bios.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: print.c
;
;  
;  Copyright (C) 2006-2011 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: ata.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: floppy.c
;
;  
;  Copyright (C) 2006-2011 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: eltorito.c
;
;  
;  Copyright (C) 2006-2013 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: boot.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: keyboard.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: disk.c
;
;  
;  Copyright (C) 2006-2011 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: serial.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: system.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: timepci.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: ps2mouse.c
;
;  
;  Copyright (C) 2006-2011 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: parallel.c
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  

;
; Source file: logo.c
;
;  $Id: VBoxBiosAlternative.asm $
;  Stuff for drawing the BIOS logo.
;  
;  
;  
;  Copyright (C) 2004-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.

;
; Source file: scsi.c
;
;  $Id: VBoxBiosAlternative.asm $
;  SCSI host adapter driver to boot from SCSI disks
;  
;  
;  
;  Copyright (C) 2004-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.

;
; Source file: ahci.c
;
;  $Id: VBoxBiosAlternative.asm $
;  AHCI host adapter driver to boot from SATA disks.
;  
;  
;  
;  Copyright (C) 2011-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.

;
; Source file: apm.c
;
;  $Id: VBoxBiosAlternative.asm $
;  APM BIOS support. Implements APM version 1.2.
;  
;  
;  
;  Copyright (C) 2004-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.

;
; Source file: pcibios.c
;
;  $Id: VBoxBiosAlternative.asm $
;  PCI BIOS support.
;  
;  
;  
;  Copyright (C) 2004-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.

;
; Source file: pciutil.c
;
;  Utility routines for calling the PCI BIOS.
;  
;  
;  
;  Copyright (C) 2011 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.

;
; Source file: vds.c
;
;  Utility routines for calling the Virtual DMA Services.
;  
;  
;  
;  Copyright (C) 2011 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.

;
; Source file: support.asm
;
;  $Id: VBoxBiosAlternative.asm $
;  Compiler support routines.
;  
;  
;  
;  Copyright (C) 2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  

;
; Source file: pcibio32.asm
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  
;  --------------------------------------------------------------------

;
; Source file: apm_pm.asm
;
;  
;  Copyright (C) 2006-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  
;  --------------------------------------------------------------------
;  
;  Protected-mode APM implementation.
;  

;
; Source file: orgs.asm
;
;  
;  Copyright (C) 2006-2011 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;  --------------------------------------------------------------------
;  
;  This code is based on:
;  
;   ROM BIOS for use with Bochs/Plex86/QEMU emulation environment
;  
;   Copyright (C) 2002  MandrakeSoft S.A.
;  
;     MandrakeSoft S.A.
;     43, rue d'Aboukir
;     75002 Paris - France
;     http://www.linux-mandrake.com/
;     http://www.mandrakesoft.com/
;  
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;  
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;  
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
;  
;  

;
; Source file: pci32.c
;
;  $Id: VBoxBiosAlternative.asm $
;  32-bit PCI BIOS wrapper.
;  
;  
;  
;  Copyright (C) 2004-2012 Oracle Corporation
;  
;  This file is part of VirtualBox Open Source Edition (OSE), as
;  available from http://www.virtualbox.org. This file is free software;
;  you can redistribute it and/or modify it under the terms of the GNU
;  General Public License (GPL) as published by the Free Software
;  Foundation, in version 2 as it comes in the "COPYING" file of the
;  VirtualBox OSE distribution. VirtualBox OSE is distributed in the
;  hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.




section _DATA progbits vstart=0x0 align=1 ; size=0x30 class=DATA group=DGROUP
_dskacc:                                     ; 0xf0000 LB 0x30
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 093h, 027h, 00dh, 028h, 000h, 000h, 000h, 000h
    db  04ch, 074h, 0f3h, 074h, 09eh, 07fh, 032h, 080h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  05fh, 033h, 032h, 05fh, 000h, 0dah, 00fh, 000h, 000h, 001h, 0f3h, 000h, 000h, 000h, 000h, 000h

section CONST progbits vstart=0x30 align=1 ; size=0xc96 class=DATA group=DGROUP
    db   'NMI Handler called', 00ah, 000h
    db   'INT18: BOOT FAILURE', 00ah, 000h
    db   '%s', 00ah, 000h, 000h
    db   'FATAL: ', 000h
    db   'bios_printf: unknown format', 00ah, 000h, 000h
    db   'ata-detect: Failed to detect ATA device', 00ah, 000h
    db   'ata%d-%d: PCHS=%u/%d/%d LCHS=%u/%u/%u', 00ah, 000h
    db   'ata-detect: Failed to detect ATAPI device', 00ah, 000h
    db   ' slave', 000h
    db   'master', 000h
    db   'ata%d %s: ', 000h
    db   '%c', 000h
    db   ' ATA-%d Hard-Disk (%lu MBytes)', 00ah, 000h
    db   ' ATAPI-%d CD-ROM/DVD-ROM', 00ah, 000h
    db   ' ATAPI-%d Device', 00ah, 000h
    db   'ata%d %s: Unknown device', 00ah, 000h
    db   'ata_cmd_packet', 000h
    db   '%s: DATA_OUT not supported yet', 00ah, 000h
    db   'set_diskette_current_cyl: drive > 1', 00ah, 000h
    db   'int13_diskette_function', 000h
    db   '%s: drive>1 || head>1 ...', 00ah, 000h
    db   '%s: ctrl not ready', 00ah, 000h
    db   '%s: read error', 00ah, 000h
    db   '%s: write error', 00ah, 000h
    db   '%s: bad floppy type', 00ah, 000h
    db   '%s: unsupported AH=%02x', 00ah, 000h, 000h
    db   'int13_eltorito', 000h
    db   '%s: call with AX=%04x not implemented.', 00ah, 000h
    db   '%s: unsupported AH=%02x', 00ah, 000h
    db   'int13_cdemu', 000h
    db   '%s: function %02x, emulation not active for DL= %02x', 00ah, 000h
    db   '%s: function %02x, error %02x !', 00ah, 000h
    db   '%s: function AH=%02x unsupported, returns fail', 00ah, 000h
    db   'int13_cdrom', 000h
    db   '%s: function %02x, ELDL out of range %02x', 00ah, 000h
    db   '%s: function %02x, unmapped device for ELDL=%02x', 00ah, 000h
    db   '%s: function %02x. Can', 027h, 't use 64bits lba', 00ah, 000h
    db   '%s: function %02x, status %02x !', 00ah, 000h, 000h
    db   'Booting from %s...', 00ah, 000h
    db   'Boot from %s failed', 00ah, 000h
    db   'Boot from %s %d failed', 00ah, 000h
    db   'No bootable medium found! System halted.', 00ah, 000h
    db   'Could not read from the boot medium! System halted.', 00ah, 000h
    db   'CDROM boot failure code : %04x', 00ah, 000h
    db   'Boot : bseqnr=%d, bootseq=%x', 00dh, 00ah, 000h, 000h
    db   'Keyboard error:%u', 00ah, 000h
    db   'KBD: int09 handler: AL=0', 00ah, 000h
    db   'KBD: int09h_handler(): unknown scancode read: 0x%02x!', 00ah, 000h
    db   'KBD: int09h_handler(): scancode & asciicode are zero?', 00ah, 000h
    db   'KBD: int16h: out of keyboard input', 00ah, 000h
    db   'KBD: unsupported int 16h function %02x', 00ah, 000h
    db   'AX=%04x BX=%04x CX=%04x DX=%04x ', 00ah, 000h, 000h
    db   'int13_harddisk', 000h
    db   '%s: function %02x, ELDL out of range %02x', 00ah, 000h
    db   '%s: function %02x, unmapped device for ELDL=%02x', 00ah, 000h
    db   '%s: function %02x, count out of range!', 00ah, 000h
    db   '%s: function %02x, disk %02x, parameters out of range %04x/%04x/%04x!', 00ah
    db   000h
    db   '%s: function %02x, error %02x !', 00ah, 000h
    db   'format disk track called', 00ah, 000h
    db   '%s: function %02xh unimplemented, returns success', 00ah, 000h
    db   '%s: function %02xh unsupported, returns fail', 00ah, 000h
    db   'int13_harddisk_ext', 000h
    db   '%s: function %02x. Can', 027h, 't use 64bits lba', 00ah, 000h
    db   '%s: function %02x. LBA out of range', 00ah, 000h
    db   'int15: Func 24h, subfunc %02xh, A20 gate control not supported', 00ah, 000h
    db   '*** int 15h function AH=bf not yet supported!', 00ah, 000h
    db   'EISA BIOS not present', 00ah, 000h
    db   '*** int 15h function AX=%04x, BX=%04x not yet supported!', 00ah, 000h
    db   'sendmouse', 000h
    db   'setkbdcomm', 000h
    db   'Mouse reset returned %02x (should be ack)', 00ah, 000h
    db   'Mouse status returned %02x (should be ack)', 00ah, 000h
    db   'INT 15h C2 AL=6, BH=%02x', 00ah, 000h
    db   'INT 15h C2 default case entered', 00ah, 000h, 000h
    db   'Key pressed: %x', 00ah, 000h
    db   00ah, 00ah, 'AHCI controller:', 00ah, 000h
    db   00ah, '    %d) Hard disk', 000h
    db   00ah, 00ah, 'SCSI controller:', 00ah, 000h
    db   'IDE controller:', 00ah, 000h
    db   00ah, '    %d) ', 000h
    db   'Secondary ', 000h
    db   'Primary ', 000h
    db   'Slave', 000h
    db   'Master', 000h
    db   'No hard disks found', 000h
    db   00ah, 000h
    db   'Press F12 to select boot device.', 00ah, 000h
    db   00ah, 'VirtualBox temporary boot device selection', 00ah, 00ah, 'Detected H'
    db   'ard disks:', 00ah, 00ah, 000h
    db   00ah, 'Other boot devices:', 00ah, ' f) Floppy', 00ah, ' c) CD-ROM', 00ah
    db   ' l) LAN', 00ah, 00ah, ' b) Continue booting', 00ah, 000h
    db   'Delaying boot for %d seconds:', 000h
    db   ' %d', 000h
    db   'scsi_read_sectors: device_id out of range %d', 00ah, 000h
    db   'scsi_write_sectors: device_id out of range %d', 00ah, 000h
    db   'scsi_enumerate_attached_devices: SCSI_INQUIRY failed', 00ah, 000h
    db   'scsi_enumerate_attached_devices: SCSI_READ_CAPACITY failed', 00ah, 000h
    db   'Disk %d has an unsupported sector size of %u', 00ah, 000h, 000h
    db   'ahci_read_sectors', 000h
    db   '%s: device_id out of range %d', 00ah, 000h
    db   'ahci_write_sectors', 000h
    db   'ahci_cmd_packet', 000h
    db   '%s: DATA_OUT not supported yet', 00ah, 000h
    db   'AHCI %d-P#%d: PCHS=%u/%d/%d LCHS=%u/%u/%u %ld sectors', 00ah, 000h, 000h
    db   'Standby', 000h
    db   'Suspend', 000h
    db   'Shutdown', 000h
    db   'APM: Unsupported function AX=%04X BX=%04X called', 00ah, 000h, 000h
    db   'PCI: Unsupported function AX=%04X BX=%04X called', 00ah, 000h

section CONST2 progbits vstart=0xcc6 align=1 ; size=0x3fa class=DATA group=DGROUP
_bios_cvs_version_string:                    ; 0xf0cc6 LB 0x12
    db  'VirtualBox 4.2.16', 000h
_bios_prefix_string:                         ; 0xf0cd8 LB 0x8
    db  'BIOS: ', 000h, 000h
_isotag:                                     ; 0xf0ce0 LB 0x6
    db  'CD001', 000h
_eltorito:                                   ; 0xf0ce6 LB 0x18
    db  'EL TORITO SPECIFICATION', 000h
_drivetypes:                                 ; 0xf0cfe LB 0x28
    db  046h, 06ch, 06fh, 070h, 070h, 079h, 000h, 000h, 000h, 000h, 048h, 061h, 072h, 064h, 020h, 044h
    db  069h, 073h, 06bh, 000h, 043h, 044h, 02dh, 052h, 04fh, 04dh, 000h, 000h, 000h, 000h, 04ch, 041h
    db  04eh, 000h, 000h, 000h, 000h, 000h, 000h, 000h
_scan_to_scanascii:                          ; 0xf0d26 LB 0x37a
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 01bh, 001h, 01bh, 001h, 01bh, 001h
    db  000h, 001h, 000h, 000h, 031h, 002h, 021h, 002h, 000h, 000h, 000h, 078h, 000h, 000h, 032h, 003h
    db  040h, 003h, 000h, 003h, 000h, 079h, 000h, 000h, 033h, 004h, 023h, 004h, 000h, 000h, 000h, 07ah
    db  000h, 000h, 034h, 005h, 024h, 005h, 000h, 000h, 000h, 07bh, 000h, 000h, 035h, 006h, 025h, 006h
    db  000h, 000h, 000h, 07ch, 000h, 000h, 036h, 007h, 05eh, 007h, 01eh, 007h, 000h, 07dh, 000h, 000h
    db  037h, 008h, 026h, 008h, 000h, 000h, 000h, 07eh, 000h, 000h, 038h, 009h, 02ah, 009h, 000h, 000h
    db  000h, 07fh, 000h, 000h, 039h, 00ah, 028h, 00ah, 000h, 000h, 000h, 080h, 000h, 000h, 030h, 00bh
    db  029h, 00bh, 000h, 000h, 000h, 081h, 000h, 000h, 02dh, 00ch, 05fh, 00ch, 01fh, 00ch, 000h, 082h
    db  000h, 000h, 03dh, 00dh, 02bh, 00dh, 000h, 000h, 000h, 083h, 000h, 000h, 008h, 00eh, 008h, 00eh
    db  07fh, 00eh, 000h, 000h, 000h, 000h, 009h, 00fh, 000h, 00fh, 000h, 000h, 000h, 000h, 000h, 000h
    db  071h, 010h, 051h, 010h, 011h, 010h, 000h, 010h, 040h, 000h, 077h, 011h, 057h, 011h, 017h, 011h
    db  000h, 011h, 040h, 000h, 065h, 012h, 045h, 012h, 005h, 012h, 000h, 012h, 040h, 000h, 072h, 013h
    db  052h, 013h, 012h, 013h, 000h, 013h, 040h, 000h, 074h, 014h, 054h, 014h, 014h, 014h, 000h, 014h
    db  040h, 000h, 079h, 015h, 059h, 015h, 019h, 015h, 000h, 015h, 040h, 000h, 075h, 016h, 055h, 016h
    db  015h, 016h, 000h, 016h, 040h, 000h, 069h, 017h, 049h, 017h, 009h, 017h, 000h, 017h, 040h, 000h
    db  06fh, 018h, 04fh, 018h, 00fh, 018h, 000h, 018h, 040h, 000h, 070h, 019h, 050h, 019h, 010h, 019h
    db  000h, 019h, 040h, 000h, 05bh, 01ah, 07bh, 01ah, 01bh, 01ah, 000h, 000h, 000h, 000h, 05dh, 01bh
    db  07dh, 01bh, 01dh, 01bh, 000h, 000h, 000h, 000h, 00dh, 01ch, 00dh, 01ch, 00ah, 01ch, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 061h, 01eh, 041h, 01eh
    db  001h, 01eh, 000h, 01eh, 040h, 000h, 073h, 01fh, 053h, 01fh, 013h, 01fh, 000h, 01fh, 040h, 000h
    db  064h, 020h, 044h, 020h, 004h, 020h, 000h, 020h, 040h, 000h, 066h, 021h, 046h, 021h, 006h, 021h
    db  000h, 021h, 040h, 000h, 067h, 022h, 047h, 022h, 007h, 022h, 000h, 022h, 040h, 000h, 068h, 023h
    db  048h, 023h, 008h, 023h, 000h, 023h, 040h, 000h, 06ah, 024h, 04ah, 024h, 00ah, 024h, 000h, 024h
    db  040h, 000h, 06bh, 025h, 04bh, 025h, 00bh, 025h, 000h, 025h, 040h, 000h, 06ch, 026h, 04ch, 026h
    db  00ch, 026h, 000h, 026h, 040h, 000h, 03bh, 027h, 03ah, 027h, 000h, 000h, 000h, 000h, 000h, 000h
    db  027h, 028h, 022h, 028h, 000h, 000h, 000h, 000h, 000h, 000h, 060h, 029h, 07eh, 029h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 05ch, 02bh
    db  07ch, 02bh, 01ch, 02bh, 000h, 000h, 000h, 000h, 07ah, 02ch, 05ah, 02ch, 01ah, 02ch, 000h, 02ch
    db  040h, 000h, 078h, 02dh, 058h, 02dh, 018h, 02dh, 000h, 02dh, 040h, 000h, 063h, 02eh, 043h, 02eh
    db  003h, 02eh, 000h, 02eh, 040h, 000h, 076h, 02fh, 056h, 02fh, 016h, 02fh, 000h, 02fh, 040h, 000h
    db  062h, 030h, 042h, 030h, 002h, 030h, 000h, 030h, 040h, 000h, 06eh, 031h, 04eh, 031h, 00eh, 031h
    db  000h, 031h, 040h, 000h, 06dh, 032h, 04dh, 032h, 00dh, 032h, 000h, 032h, 040h, 000h, 02ch, 033h
    db  03ch, 033h, 000h, 000h, 000h, 000h, 000h, 000h, 02eh, 034h, 03eh, 034h, 000h, 000h, 000h, 000h
    db  000h, 000h, 02fh, 035h, 03fh, 035h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 02ah, 037h, 02ah, 037h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 020h, 039h, 020h, 039h, 020h, 039h
    db  020h, 039h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 03bh
    db  000h, 054h, 000h, 05eh, 000h, 068h, 000h, 000h, 000h, 03ch, 000h, 055h, 000h, 05fh, 000h, 069h
    db  000h, 000h, 000h, 03dh, 000h, 056h, 000h, 060h, 000h, 06ah, 000h, 000h, 000h, 03eh, 000h, 057h
    db  000h, 061h, 000h, 06bh, 000h, 000h, 000h, 03fh, 000h, 058h, 000h, 062h, 000h, 06ch, 000h, 000h
    db  000h, 040h, 000h, 059h, 000h, 063h, 000h, 06dh, 000h, 000h, 000h, 041h, 000h, 05ah, 000h, 064h
    db  000h, 06eh, 000h, 000h, 000h, 042h, 000h, 05bh, 000h, 065h, 000h, 06fh, 000h, 000h, 000h, 043h
    db  000h, 05ch, 000h, 066h, 000h, 070h, 000h, 000h, 000h, 044h, 000h, 05dh, 000h, 067h, 000h, 071h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 047h, 037h, 047h, 000h, 077h, 000h, 000h, 020h, 000h
    db  000h, 048h, 038h, 048h, 000h, 000h, 000h, 000h, 020h, 000h, 000h, 049h, 039h, 049h, 000h, 084h
    db  000h, 000h, 020h, 000h, 02dh, 04ah, 02dh, 04ah, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 04bh
    db  034h, 04bh, 000h, 073h, 000h, 000h, 020h, 000h, 000h, 04ch, 035h, 04ch, 000h, 000h, 000h, 000h
    db  020h, 000h, 000h, 04dh, 036h, 04dh, 000h, 074h, 000h, 000h, 020h, 000h, 02bh, 04eh, 02bh, 04eh
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 04fh, 031h, 04fh, 000h, 075h, 000h, 000h, 020h, 000h
    db  000h, 050h, 032h, 050h, 000h, 000h, 000h, 000h, 020h, 000h, 000h, 051h, 033h, 051h, 000h, 076h
    db  000h, 000h, 020h, 000h, 000h, 052h, 030h, 052h, 000h, 000h, 000h, 000h, 020h, 000h, 000h, 053h
    db  02eh, 053h, 000h, 000h, 000h, 000h, 020h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 05ch, 056h, 07ch, 056h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 085h, 000h, 087h, 000h, 089h, 000h, 08bh, 000h, 000h
    db  000h, 086h, 000h, 088h, 000h, 08ah, 000h, 08ch, 000h, 000h
_panic_msg_keyb_buffer_full:                 ; 0xf10a0 LB 0x20
    db  '%s: keyboard input buffer full', 00ah, 000h

  ; Padding 0x540 bytes at 0xf10c0
  times 1344 db 0

section _TEXT progbits vstart=0x1600 align=1 ; size=0x78d5 class=CODE group=AUTO
read_byte_:                                  ; 0xf1600 LB 0xe
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, dx                                ; 89 d3
    mov es, ax                                ; 8e c0
    mov al, byte [es:bx]                      ; 26 8a 07
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
write_byte_:                                 ; 0xf160e LB 0xe
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov si, dx                                ; 89 d6
    mov es, ax                                ; 8e c0
    mov byte [es:si], bl                      ; 26 88 1c
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn                                      ; c3
read_word_:                                  ; 0xf161c LB 0xe
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, dx                                ; 89 d3
    mov es, ax                                ; 8e c0
    mov ax, word [es:bx]                      ; 26 8b 07
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
write_word_:                                 ; 0xf162a LB 0xe
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov si, dx                                ; 89 d6
    mov es, ax                                ; 8e c0
    mov word [es:si], bx                      ; 26 89 1c
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn                                      ; c3
read_dword_:                                 ; 0xf1638 LB 0x12
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, dx                                ; 89 d3
    mov es, ax                                ; 8e c0
    mov ax, word [es:bx]                      ; 26 8b 07
    mov dx, word [es:bx+002h]                 ; 26 8b 57 02
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
write_dword_:                                ; 0xf164a LB 0x12
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov si, dx                                ; 89 d6
    mov es, ax                                ; 8e c0
    mov word [es:si], bx                      ; 26 89 1c
    mov word [es:si+002h], cx                 ; 26 89 4c 02
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn                                      ; c3
inb_cmos_:                                   ; 0xf165c LB 0x14
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    mov dx, strict word 00070h                ; ba 70 00
    out DX, AL                                ; ee
    mov dx, strict word 00071h                ; ba 71 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
outb_cmos_:                                  ; 0xf1670 LB 0x13
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov ah, dl                                ; 88 d4
    mov dx, strict word 00070h                ; ba 70 00
    out DX, AL                                ; ee
    mov al, ah                                ; 88 e0
    mov dx, strict word 00071h                ; ba 71 00
    out DX, AL                                ; ee
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_dummy_isr_function:                         ; 0xf1683 LB 0x6b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push ax                                   ; 50
    mov CL, strict byte 0ffh                  ; b1 ff
    mov AL, strict byte 00bh                  ; b0 0b
    mov dx, strict word 00020h                ; ba 20 00
    out DX, AL                                ; ee
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov bx, ax                                ; 89 c3
    mov byte [bp-002h], al                    ; 88 46 fe
    test al, al                               ; 84 c0
    je short 016deh                           ; 74 43
    mov AL, strict byte 00bh                  ; b0 0b
    mov dx, 000a0h                            ; ba a0 00
    out DX, AL                                ; ee
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov cx, ax                                ; 89 c1
    test al, al                               ; 84 c0
    je short 016c0h                           ; 74 16
    mov dx, 000a1h                            ; ba a1 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    xor ah, ah                                ; 30 e4
    movzx bx, cl                              ; 0f b6 d9
    or ax, bx                                 ; 09 d8
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    mov dx, 000a0h                            ; ba a0 00
    out DX, AL                                ; ee
    jmp short 016d5h                          ; eb 15
    mov dx, strict word 00021h                ; ba 21 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and bl, 0fbh                              ; 80 e3 fb
    mov byte [bp-002h], bl                    ; 88 5e fe
    xor ah, ah                                ; 30 e4
    movzx bx, byte [bp-002h]                  ; 0f b6 5e fe
    or ax, bx                                 ; 09 d8
    out DX, AL                                ; ee
    mov AL, strict byte 020h                  ; b0 20
    mov dx, strict word 00020h                ; ba 20 00
    out DX, AL                                ; ee
    mov cl, byte [bp-002h]                    ; 8a 4e fe
    movzx bx, cl                              ; 0f b6 d9
    mov dx, strict word 0006bh                ; ba 6b 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 24 ff
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_nmi_handler_msg:                            ; 0xf16ee LB 0x12
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push strict word 00030h                   ; 68 30 00
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 14 02
    add sp, strict byte 00004h                ; 83 c4 04
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_int18_panic_msg:                            ; 0xf1700 LB 0x12
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push strict word 00044h                   ; 68 44 00
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 02 02
    add sp, strict byte 00004h                ; 83 c4 04
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_log_bios_start:                             ; 0xf1712 LB 0x20
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 ac 01
    push 00cc6h                               ; 68 c6 0c
    push strict word 00059h                   ; 68 59 00
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 e2 01
    add sp, strict byte 00006h                ; 83 c4 06
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_print_bios_banner:                          ; 0xf1732 LB 0x2e
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov dx, strict word 00072h                ; ba 72 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 de fe
    mov cx, ax                                ; 89 c1
    xor bx, bx                                ; 31 db
    mov dx, strict word 00072h                ; ba 72 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0162ah                               ; e8 df fe
    cmp cx, 01234h                            ; 81 f9 34 12
    jne short 01759h                          ; 75 08
    mov AL, strict byte 003h                  ; b0 03
    mov AH, strict byte 000h                  ; b4 00
    int 010h                                  ; cd 10
    jmp short 0175ch                          ; eb 03
    call 070f2h                               ; e8 96 59
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
send_:                                       ; 0xf1760 LB 0x3b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    mov bx, ax                                ; 89 c3
    mov cl, dl                                ; 88 d1
    test AL, strict byte 008h                 ; a8 08
    je short 01773h                           ; 74 06
    mov al, dl                                ; 88 d0
    mov dx, 00403h                            ; ba 03 04
    out DX, AL                                ; ee
    test bl, 004h                             ; f6 c3 04
    je short 0177eh                           ; 74 06
    mov al, cl                                ; 88 c8
    mov dx, 00504h                            ; ba 04 05
    out DX, AL                                ; ee
    test bl, 002h                             ; f6 c3 02
    je short 01794h                           ; 74 11
    cmp cl, 00ah                              ; 80 f9 0a
    jne short 0178eh                          ; 75 06
    mov AL, strict byte 00dh                  ; b0 0d
    mov AH, strict byte 00eh                  ; b4 0e
    int 010h                                  ; cd 10
    mov al, cl                                ; 88 c8
    mov AH, strict byte 00eh                  ; b4 0e
    int 010h                                  ; cd 10
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
put_int_:                                    ; 0xf179b LB 0x5f
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov si, ax                                ; 89 c6
    mov word [bp-008h], dx                    ; 89 56 f8
    mov di, strict word 0000ah                ; bf 0a 00
    mov ax, dx                                ; 89 d0
    cwd                                       ; 99
    idiv di                                   ; f7 ff
    mov word [bp-006h], ax                    ; 89 46 fa
    test ax, ax                               ; 85 c0
    je short 017c0h                           ; 74 0a
    dec bx                                    ; 4b
    mov dx, ax                                ; 89 c2
    mov ax, si                                ; 89 f0
    call 0179bh                               ; e8 dd ff
    jmp short 017dbh                          ; eb 1b
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jle short 017cfh                          ; 7e 0a
    mov dx, strict word 00020h                ; ba 20 00
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 93 ff
    jmp short 017c0h                          ; eb f1
    test cx, cx                               ; 85 c9
    je short 017dbh                           ; 74 08
    mov dx, strict word 0002dh                ; ba 2d 00
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 85 ff
    mov al, byte [bp-006h]                    ; 8a 46 fa
    mov DL, strict byte 00ah                  ; b2 0a
    mul dl                                    ; f6 e2
    mov dl, byte [bp-008h]                    ; 8a 56 f8
    sub dl, al                                ; 28 c2
    mov al, dl                                ; 88 d0
    add AL, strict byte 030h                  ; 04 30
    movzx dx, al                              ; 0f b6 d0
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 6d ff
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
put_uint_:                                   ; 0xf17fa LB 0x60
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov si, ax                                ; 89 c6
    mov word [bp-008h], dx                    ; 89 56 f8
    mov ax, dx                                ; 89 d0
    xor dx, dx                                ; 31 d2
    mov di, strict word 0000ah                ; bf 0a 00
    div di                                    ; f7 f7
    mov word [bp-006h], ax                    ; 89 46 fa
    test ax, ax                               ; 85 c0
    je short 01820h                           ; 74 0a
    dec bx                                    ; 4b
    mov dx, ax                                ; 89 c2
    mov ax, si                                ; 89 f0
    call 017fah                               ; e8 dc ff
    jmp short 0183bh                          ; eb 1b
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jle short 0182fh                          ; 7e 0a
    mov dx, strict word 00020h                ; ba 20 00
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 33 ff
    jmp short 01820h                          ; eb f1
    test cx, cx                               ; 85 c9
    je short 0183bh                           ; 74 08
    mov dx, strict word 0002dh                ; ba 2d 00
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 25 ff
    mov al, byte [bp-006h]                    ; 8a 46 fa
    mov DL, strict byte 00ah                  ; b2 0a
    mul dl                                    ; f6 e2
    mov dl, byte [bp-008h]                    ; 8a 56 f8
    sub dl, al                                ; 28 c2
    mov al, dl                                ; 88 d0
    add AL, strict byte 030h                  ; 04 30
    movzx dx, al                              ; 0f b6 d0
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 0d ff
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
put_luint_:                                  ; 0xf185a LB 0x72
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov si, ax                                ; 89 c6
    mov word [bp-006h], bx                    ; 89 5e fa
    mov di, dx                                ; 89 d7
    mov ax, bx                                ; 89 d8
    mov dx, cx                                ; 89 ca
    mov bx, strict word 0000ah                ; bb 0a 00
    xor cx, cx                                ; 31 c9
    call 08de0h                               ; e8 6c 75
    mov word [bp-008h], ax                    ; 89 46 f8
    mov cx, dx                                ; 89 d1
    mov dx, ax                                ; 89 c2
    or dx, cx                                 ; 09 ca
    je short 0188eh                           ; 74 0f
    push word [bp+004h]                       ; ff 76 04
    lea dx, [di-001h]                         ; 8d 55 ff
    mov bx, ax                                ; 89 c3
    mov ax, si                                ; 89 f0
    call 0185ah                               ; e8 ce ff
    jmp short 018abh                          ; eb 1d
    dec di                                    ; 4f
    test di, di                               ; 85 ff
    jle short 0189dh                          ; 7e 0a
    mov dx, strict word 00020h                ; ba 20 00
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 c5 fe
    jmp short 0188eh                          ; eb f1
    cmp word [bp+004h], strict byte 00000h    ; 83 7e 04 00
    je short 018abh                           ; 74 08
    mov dx, strict word 0002dh                ; ba 2d 00
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 b5 fe
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov DL, strict byte 00ah                  ; b2 0a
    mul dl                                    ; f6 e2
    mov dl, byte [bp-006h]                    ; 8a 56 fa
    sub dl, al                                ; 28 c2
    mov al, dl                                ; 88 d0
    add AL, strict byte 030h                  ; 04 30
    movzx dx, al                              ; 0f b6 d0
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 9d fe
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00002h                               ; c2 02 00
put_str_:                                    ; 0xf18cc LB 0x21
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    push si                                   ; 56
    mov si, ax                                ; 89 c6
    mov es, cx                                ; 8e c1
    mov dl, byte [es:bx]                      ; 26 8a 17
    test dl, dl                               ; 84 d2
    je short 018e6h                           ; 74 0a
    xor dh, dh                                ; 30 f6
    mov ax, si                                ; 89 f0
    call 01760h                               ; e8 7d fe
    inc bx                                    ; 43
    jmp short 018d3h                          ; eb ed
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
put_str_near_:                               ; 0xf18ed LB 0x20
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    mov cx, ax                                ; 89 c1
    mov bx, dx                                ; 89 d3
    mov dl, byte [bx]                         ; 8a 17
    test dl, dl                               ; 84 d2
    je short 01906h                           ; 74 0a
    xor dh, dh                                ; 30 f6
    mov ax, cx                                ; 89 c8
    call 01760h                               ; e8 5d fe
    inc bx                                    ; 43
    jmp short 018f6h                          ; eb f0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
bios_printf_:                                ; 0xf190d LB 0x23b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00008h                ; 83 ec 08
    lea bx, [bp+008h]                         ; 8d 5e 08
    mov word [bp-012h], bx                    ; 89 5e ee
    mov [bp-010h], ss                         ; 8c 56 f0
    xor cx, cx                                ; 31 c9
    xor si, si                                ; 31 f6
    mov ax, word [bp+004h]                    ; 8b 46 04
    and ax, strict word 00007h                ; 25 07 00
    cmp ax, strict word 00007h                ; 3d 07 00
    jne short 0193bh                          ; 75 0b
    push strict word 0005eh                   ; 68 5e 00
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 d5 ff
    add sp, strict byte 00004h                ; 83 c4 04
    mov bx, word [bp+006h]                    ; 8b 5e 06
    mov dl, byte [bx]                         ; 8a 17
    test dl, dl                               ; 84 d2
    je near 01b2ch                            ; 0f 84 e6 01
    cmp dl, 025h                              ; 80 fa 25
    jne short 01953h                          ; 75 08
    mov cx, strict word 00001h                ; b9 01 00
    xor si, si                                ; 31 f6
    jmp near 01b26h                           ; e9 d3 01
    test cx, cx                               ; 85 c9
    je near 01b1eh                            ; 0f 84 c5 01
    cmp dl, 030h                              ; 80 fa 30
    jc short 01971h                           ; 72 13
    cmp dl, 039h                              ; 80 fa 39
    jnbe short 01971h                         ; 77 0e
    movzx ax, dl                              ; 0f b6 c2
    imul si, si, strict byte 0000ah           ; 6b f6 0a
    sub ax, strict word 00030h                ; 2d 30 00
    add si, ax                                ; 01 c6
    jmp near 01b26h                           ; e9 b5 01
    mov ax, word [bp-010h]                    ; 8b 46 f0
    mov word [bp-010h], ax                    ; 89 46 f0
    add word [bp-012h], strict byte 00002h    ; 83 46 ee 02
    les bx, [bp-012h]                         ; c4 5e ee
    mov ax, word [es:bx-002h]                 ; 26 8b 47 fe
    mov word [bp-00ch], ax                    ; 89 46 f4
    cmp dl, 078h                              ; 80 fa 78
    je short 0198fh                           ; 74 05
    cmp dl, 058h                              ; 80 fa 58
    jne short 019d8h                          ; 75 49
    test si, si                               ; 85 f6
    jne short 01996h                          ; 75 03
    mov si, strict word 00004h                ; be 04 00
    cmp dl, 078h                              ; 80 fa 78
    jne short 019a0h                          ; 75 05
    mov di, strict word 00061h                ; bf 61 00
    jmp short 019a3h                          ; eb 03
    mov di, strict word 00041h                ; bf 41 00
    lea bx, [si-001h]                         ; 8d 5c ff
    test bx, bx                               ; 85 db
    jl near 01b1ah                            ; 0f 8c 6e 01
    mov cx, bx                                ; 89 d9
    sal cx, 002h                              ; c1 e1 02
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    shr ax, CL                                ; d3 e8
    xor ah, ah                                ; 30 e4
    and AL, strict byte 00fh                  ; 24 0f
    cmp ax, strict word 00009h                ; 3d 09 00
    jnbe short 019c6h                         ; 77 07
    mov dx, ax                                ; 89 c2
    add dx, strict byte 00030h                ; 83 c2 30
    jmp short 019cdh                          ; eb 07
    mov dx, ax                                ; 89 c2
    sub dx, strict byte 0000ah                ; 83 ea 0a
    add dx, di                                ; 01 fa
    xor dh, dh                                ; 30 f6
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 01760h                               ; e8 8b fd
    dec bx                                    ; 4b
    jmp short 019a6h                          ; eb ce
    cmp dl, 075h                              ; 80 fa 75
    jne short 019ech                          ; 75 0f
    xor cx, cx                                ; 31 c9
    mov bx, si                                ; 89 f3
    mov dx, ax                                ; 89 c2
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 017fah                               ; e8 11 fe
    jmp near 01b1ah                           ; e9 2e 01
    lea bx, [si-001h]                         ; 8d 5c ff
    cmp dl, 06ch                              ; 80 fa 6c
    jne near 01aa8h                           ; 0f 85 b2 00
    inc word [bp+006h]                        ; ff 46 06
    mov di, word [bp+006h]                    ; 8b 7e 06
    mov dl, byte [di]                         ; 8a 15
    mov ax, word [bp-010h]                    ; 8b 46 f0
    mov word [bp-010h], ax                    ; 89 46 f0
    add word [bp-012h], strict byte 00002h    ; 83 46 ee 02
    les di, [bp-012h]                         ; c4 7e ee
    mov ax, word [es:di-002h]                 ; 26 8b 45 fe
    mov word [bp-00eh], ax                    ; 89 46 f2
    cmp dl, 064h                              ; 80 fa 64
    jne short 01a44h                          ; 75 2d
    test byte [bp-00dh], 080h                 ; f6 46 f3 80
    je short 01a32h                           ; 74 15
    push strict byte 00001h                   ; 6a 01
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    mov cx, word [bp-00eh]                    ; 8b 4e f2
    neg cx                                    ; f7 d9
    neg ax                                    ; f7 d8
    sbb cx, strict byte 00000h                ; 83 d9 00
    mov dx, bx                                ; 89 da
    mov bx, ax                                ; 89 c3
    jmp short 01a3bh                          ; eb 09
    push strict byte 00000h                   ; 6a 00
    mov bx, word [bp-00ch]                    ; 8b 5e f4
    mov dx, si                                ; 89 f2
    mov cx, ax                                ; 89 c1
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0185ah                               ; e8 19 fe
    jmp near 01b1ah                           ; e9 d6 00
    cmp dl, 075h                              ; 80 fa 75
    jne short 01a4bh                          ; 75 02
    jmp short 01a32h                          ; eb e7
    cmp dl, 078h                              ; 80 fa 78
    je short 01a57h                           ; 74 07
    cmp dl, 058h                              ; 80 fa 58
    jne near 01b1ah                           ; 0f 85 c3 00
    test si, si                               ; 85 f6
    jne short 01a5eh                          ; 75 03
    mov si, strict word 00008h                ; be 08 00
    cmp dl, 078h                              ; 80 fa 78
    jne short 01a68h                          ; 75 05
    mov di, strict word 00061h                ; bf 61 00
    jmp short 01a6bh                          ; eb 03
    mov di, strict word 00041h                ; bf 41 00
    lea bx, [si-001h]                         ; 8d 5c ff
    test bx, bx                               ; 85 db
    jl near 01b1ah                            ; 0f 8c a6 00
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    mov cx, bx                                ; 89 d9
    sal cx, 002h                              ; c1 e1 02
    mov dx, word [bp-00eh]                    ; 8b 56 f2
    jcxz 01a87h                               ; e3 06
    shr dx, 1                                 ; d1 ea
    rcr ax, 1                                 ; d1 d8
    loop 01a81h                               ; e2 fa
    and ax, strict word 0000fh                ; 25 0f 00
    cmp ax, strict word 00009h                ; 3d 09 00
    jnbe short 01a96h                         ; 77 07
    mov dx, ax                                ; 89 c2
    add dx, strict byte 00030h                ; 83 c2 30
    jmp short 01a9dh                          ; eb 07
    mov dx, ax                                ; 89 c2
    sub dx, strict byte 0000ah                ; 83 ea 0a
    add dx, di                                ; 01 fa
    xor dh, dh                                ; 30 f6
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 01760h                               ; e8 bb fc
    dec bx                                    ; 4b
    jmp short 01a6eh                          ; eb c6
    cmp dl, 064h                              ; 80 fa 64
    jne short 01acah                          ; 75 1d
    test byte [bp-00bh], 080h                 ; f6 46 f5 80
    je short 01abch                           ; 74 09
    mov dx, ax                                ; 89 c2
    neg dx                                    ; f7 da
    mov cx, strict word 00001h                ; b9 01 00
    jmp short 01ac2h                          ; eb 06
    xor cx, cx                                ; 31 c9
    mov bx, si                                ; 89 f3
    mov dx, ax                                ; 89 c2
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0179bh                               ; e8 d3 fc
    jmp short 01b1ah                          ; eb 50
    cmp dl, 073h                              ; 80 fa 73
    jne short 01adbh                          ; 75 0c
    mov cx, ds                                ; 8c d9
    mov bx, ax                                ; 89 c3
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 018cch                               ; e8 f3 fd
    jmp short 01b1ah                          ; eb 3f
    cmp dl, 053h                              ; 80 fa 53
    jne short 01afeh                          ; 75 1e
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov ax, word [bp-010h]                    ; 8b 46 f0
    mov word [bp-010h], ax                    ; 89 46 f0
    add word [bp-012h], strict byte 00002h    ; 83 46 ee 02
    les bx, [bp-012h]                         ; c4 5e ee
    mov ax, word [es:bx-002h]                 ; 26 8b 47 fe
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov bx, ax                                ; 89 c3
    mov cx, word [bp-00eh]                    ; 8b 4e f2
    jmp short 01ad3h                          ; eb d5
    cmp dl, 063h                              ; 80 fa 63
    jne short 01b0fh                          ; 75 0c
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 01760h                               ; e8 53 fc
    jmp short 01b1ah                          ; eb 0b
    push strict word 00066h                   ; 68 66 00
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 f6 fd
    add sp, strict byte 00004h                ; 83 c4 04
    xor cx, cx                                ; 31 c9
    jmp short 01b26h                          ; eb 08
    xor dh, dh                                ; 30 f6
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 01760h                               ; e8 3a fc
    inc word [bp+006h]                        ; ff 46 06
    jmp near 0193bh                           ; e9 0f fe
    xor ax, ax                                ; 31 c0
    mov word [bp-012h], ax                    ; 89 46 ee
    mov word [bp-010h], ax                    ; 89 46 f0
    test byte [bp+004h], 001h                 ; f6 46 04 01
    je short 01b3eh                           ; 74 04
    cli                                       ; fa
    hlt                                       ; f4
    jmp short 01b3bh                          ; eb fd
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
_ata_init:                                   ; 0xf1b48 LB 0xc4
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 c7 fa
    mov si, 00122h                            ; be 22 01
    mov dx, ax                                ; 89 c2
    xor al, al                                ; 30 c0
    jmp short 01b62h                          ; eb 04
    cmp AL, strict byte 004h                  ; 3c 04
    jnc short 01b86h                          ; 73 24
    movzx bx, al                              ; 0f b6 d8
    imul bx, bx, strict byte 00006h           ; 6b db 06
    mov es, dx                                ; 8e c2
    add bx, si                                ; 01 f3
    mov byte [es:bx+001c0h], 000h             ; 26 c6 87 c0 01 00
    db  066h, 026h, 0c7h, 087h, 0c2h, 001h, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+001c2h], strict dword 000000000h ; 66 26 c7 87 c2 01 00 00 00 00
    mov byte [es:bx+001c1h], 000h             ; 26 c6 87 c1 01 00
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    jmp short 01b5eh                          ; eb d8
    xor al, al                                ; 30 c0
    jmp short 01b8eh                          ; eb 04
    cmp AL, strict byte 008h                  ; 3c 08
    jnc short 01bd9h                          ; 73 4b
    movzx bx, al                              ; 0f b6 d8
    imul bx, bx, strict byte 00018h           ; 6b db 18
    mov es, dx                                ; 8e c2
    add bx, si                                ; 01 f3
    db  066h, 026h, 0c7h, 047h, 01eh, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+01eh], strict dword 000000000h ; 66 26 c7 47 1e 00 00 00 00
    mov byte [es:bx+022h], 000h               ; 26 c6 47 22 00
    mov word [es:bx+024h], 00200h             ; 26 c7 47 24 00 02
    mov byte [es:bx+023h], 000h               ; 26 c6 47 23 00
    db  066h, 026h, 0c7h, 047h, 026h, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+026h], strict dword 000000000h ; 66 26 c7 47 26 00 00 00 00
    db  066h, 026h, 0c7h, 047h, 02ah, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+02ah], strict dword 000000000h ; 66 26 c7 47 2a 00 00 00 00
    db  066h, 026h, 0c7h, 047h, 02eh, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+02eh], strict dword 000000000h ; 66 26 c7 47 2e 00 00 00 00
    db  066h, 026h, 0c7h, 047h, 032h, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+032h], strict dword 000000000h ; 66 26 c7 47 32 00 00 00 00
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    jmp short 01b8ah                          ; eb b1
    xor al, al                                ; 30 c0
    jmp short 01be1h                          ; eb 04
    cmp AL, strict byte 010h                  ; 3c 10
    jnc short 01bf8h                          ; 73 17
    movzx bx, al                              ; 0f b6 d8
    mov es, dx                                ; 8e c2
    add bx, si                                ; 01 f3
    mov byte [es:bx+0019fh], 010h             ; 26 c6 87 9f 01 10
    mov byte [es:bx+001b0h], 010h             ; 26 c6 87 b0 01 10
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    jmp short 01bddh                          ; eb e5
    mov es, dx                                ; 8e c2
    mov byte [es:si+0019eh], 000h             ; 26 c6 84 9e 01 00
    mov byte [es:si+001afh], 000h             ; 26 c6 84 af 01 00
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
ata_reset_:                                  ; 0xf1c0c LB 0xde
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    push ax                                   ; 50
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 fc f9
    mov word [bp-00eh], 00122h                ; c7 46 f2 22 01
    mov di, ax                                ; 89 c7
    mov bx, word [bp-010h]                    ; 8b 5e f0
    shr bx, 1                                 ; d1 eb
    mov dl, byte [bp-010h]                    ; 8a 56 f0
    and dl, 001h                              ; 80 e2 01
    mov byte [bp-00ch], dl                    ; 88 56 f4
    xor bh, bh                                ; 30 ff
    imul bx, bx, strict byte 00006h           ; 6b db 06
    mov es, ax                                ; 8e c0
    add bx, 00122h                            ; 81 c3 22 01
    mov cx, word [es:bx+001c2h]               ; 26 8b 8f c2 01
    mov si, word [es:bx+001c4h]               ; 26 8b b7 c4 01
    lea dx, [si+006h]                         ; 8d 54 06
    mov AL, strict byte 00eh                  ; b0 0e
    out DX, AL                                ; ee
    mov bx, 000ffh                            ; bb ff 00
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 01c64h                          ; 76 0c
    mov dx, cx                                ; 89 ca
    add dx, strict byte 00007h                ; 83 c2 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 080h                 ; a8 80
    je short 01c53h                           ; 74 ef
    lea dx, [si+006h]                         ; 8d 54 06
    mov AL, strict byte 00ah                  ; b0 0a
    out DX, AL                                ; ee
    imul bx, word [bp-010h], strict byte 00018h ; 6b 5e f0 18
    mov es, di                                ; 8e c7
    add bx, word [bp-00eh]                    ; 03 5e f2
    cmp byte [es:bx+01eh], 000h               ; 26 80 7f 1e 00
    je short 01cc6h                           ; 74 4c
    cmp byte [bp-00ch], 000h                  ; 80 7e f4 00
    je short 01c85h                           ; 74 05
    mov ax, 000b0h                            ; b8 b0 00
    jmp short 01c88h                          ; eb 03
    mov ax, 000a0h                            ; b8 a0 00
    mov dx, cx                                ; 89 ca
    add dx, strict byte 00006h                ; 83 c2 06
    out DX, AL                                ; ee
    mov dx, cx                                ; 89 ca
    inc dx                                    ; 42
    inc dx                                    ; 42
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov bx, ax                                ; 89 c3
    mov dx, cx                                ; 89 ca
    add dx, strict byte 00003h                ; 83 c2 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp bl, 001h                              ; 80 fb 01
    jne short 01cc6h                          ; 75 22
    cmp al, bl                                ; 38 d8
    jne short 01cc6h                          ; 75 1e
    mov bx, strict word 0ffffh                ; bb ff ff
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 01cc6h                          ; 76 16
    mov dx, cx                                ; 89 ca
    add dx, strict byte 00007h                ; 83 c2 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 080h                 ; a8 80
    je short 01cc6h                           ; 74 0a
    mov ax, strict word 0ffffh                ; b8 ff ff
    dec ax                                    ; 48
    test ax, ax                               ; 85 c0
    jnbe short 01cbfh                         ; 77 fb
    jmp short 01cabh                          ; eb e5
    mov bx, strict word 00010h                ; bb 10 00
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 01cdah                          ; 76 0c
    mov dx, cx                                ; 89 ca
    add dx, strict byte 00007h                ; 83 c2 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 040h                 ; a8 40
    je short 01cc9h                           ; 74 ef
    lea dx, [si+006h]                         ; 8d 54 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
ata_cmd_data_in_:                            ; 0xf1cea LB 0x268
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0001ch                ; 83 ec 1c
    mov si, ax                                ; 89 c6
    mov word [bp-00ah], dx                    ; 89 56 f6
    mov word [bp-016h], bx                    ; 89 5e ea
    mov word [bp-014h], cx                    ; 89 4e ec
    mov es, dx                                ; 8e c2
    movzx ax, byte [es:si+008h]               ; 26 0f b6 44 08
    mov dx, ax                                ; 89 c2
    shr dx, 1                                 ; d1 ea
    mov dh, al                                ; 88 c6
    and dh, 001h                              ; 80 e6 01
    mov byte [bp-008h], dh                    ; 88 76 f8
    movzx di, dl                              ; 0f b6 fa
    imul di, di, strict byte 00006h           ; 6b ff 06
    add di, si                                ; 01 f7
    mov bx, word [es:di+001c2h]               ; 26 8b 9d c2 01
    mov dx, word [es:di+001c4h]               ; 26 8b 95 c4 01
    mov word [bp-01ch], dx                    ; 89 56 e4
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov di, si                                ; 89 f7
    add di, ax                                ; 01 c7
    mov al, byte [es:di+022h]                 ; 26 8a 45 22
    mov byte [bp-006h], al                    ; 88 46 fa
    mov ax, word [es:di+024h]                 ; 26 8b 45 24
    mov word [bp-00ch], ax                    ; 89 46 f4
    test ax, ax                               ; 85 c0
    jne short 01d52h                          ; 75 14
    cmp byte [bp-006h], 001h                  ; 80 7e fa 01
    jne short 01d4bh                          ; 75 07
    mov word [bp-00ch], 04000h                ; c7 46 f4 00 40
    jmp short 01d61h                          ; eb 16
    mov word [bp-00ch], 08000h                ; c7 46 f4 00 80
    jmp short 01d61h                          ; eb 0f
    cmp byte [bp-006h], 001h                  ; 80 7e fa 01
    jne short 01d5eh                          ; 75 06
    shr word [bp-00ch], 002h                  ; c1 6e f4 02
    jmp short 01d61h                          ; eb 03
    shr word [bp-00ch], 1                     ; d1 6e f4
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 080h                 ; a8 80
    je short 01d7ah                           ; 74 0f
    mov dx, word [bp-01ch]                    ; 8b 56 e4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00001h                ; ba 01 00
    jmp near 01f39h                           ; e9 bf 01
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov ax, word [es:si]                      ; 26 8b 04
    mov word [bp-020h], ax                    ; 89 46 e0
    mov ax, word [es:si+002h]                 ; 26 8b 44 02
    mov word [bp-01eh], ax                    ; 89 46 e2
    mov di, word [es:si+004h]                 ; 26 8b 7c 04
    mov ax, word [es:si+006h]                 ; 26 8b 44 06
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov ax, word [es:si+012h]                 ; 26 8b 44 12
    mov word [bp-010h], ax                    ; 89 46 f0
    mov ax, word [es:si+00eh]                 ; 26 8b 44 0e
    mov word [bp-012h], ax                    ; 89 46 ee
    mov ax, word [es:si+010h]                 ; 26 8b 44 10
    mov word [bp-018h], ax                    ; 89 46 e8
    mov ax, word [bp-010h]                    ; 8b 46 f0
    test ax, ax                               ; 85 c0
    jne short 01e18h                          ; 75 67
    mov dx, word [bp-020h]                    ; 8b 56 e0
    add dx, word [bp-014h]                    ; 03 56 ec
    adc ax, word [bp-01eh]                    ; 13 46 e2
    cmp ax, 01000h                            ; 3d 00 10
    jnbe short 01dc1h                         ; 77 02
    jne short 01dech                          ; 75 2b
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov word [bp-010h], ax                    ; 89 46 f0
    mov ax, word [bp-014h]                    ; 8b 46 ec
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    lea dx, [bx+002h]                         ; 8d 57 02
    out DX, AL                                ; ee
    lea dx, [bx+003h]                         ; 8d 57 03
    mov al, byte [bp-010h]                    ; 8a 46 f0
    out DX, AL                                ; ee
    lea dx, [bx+004h]                         ; 8d 57 04
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    lea dx, [bx+005h]                         ; 8d 57 05
    out DX, AL                                ; ee
    mov byte [bp-01dh], al                    ; 88 46 e3
    mov ax, word [bp-020h]                    ; 8b 46 e0
    xor ah, ah                                ; 30 e4
    mov word [bp-010h], ax                    ; 89 46 f0
    mov cx, strict word 00008h                ; b9 08 00
    shr word [bp-01eh], 1                     ; d1 6e e2
    rcr word [bp-020h], 1                     ; d1 5e e0
    loop 01df7h                               ; e2 f8
    mov ax, word [bp-020h]                    ; 8b 46 e0
    mov word [bp-012h], ax                    ; 89 46 ee
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    mov word [bp-020h], ax                    ; 89 46 e0
    mov word [bp-01eh], strict word 00000h    ; c7 46 e2 00 00
    and ax, strict word 0000fh                ; 25 0f 00
    or AL, strict byte 040h                   ; 0c 40
    mov word [bp-018h], ax                    ; 89 46 e8
    mov dx, word [bp-01ch]                    ; 8b 56 e4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 00ah                  ; b0 0a
    out DX, AL                                ; ee
    lea dx, [bx+001h]                         ; 8d 57 01
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    lea dx, [bx+002h]                         ; 8d 57 02
    mov al, byte [bp-014h]                    ; 8a 46 ec
    out DX, AL                                ; ee
    lea dx, [bx+003h]                         ; 8d 57 03
    mov al, byte [bp-010h]                    ; 8a 46 f0
    out DX, AL                                ; ee
    mov ax, word [bp-012h]                    ; 8b 46 ee
    lea dx, [bx+004h]                         ; 8d 57 04
    out DX, AL                                ; ee
    shr ax, 008h                              ; c1 e8 08
    lea dx, [bx+005h]                         ; 8d 57 05
    out DX, AL                                ; ee
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    je short 01e4eh                           ; 74 05
    mov ax, 000b0h                            ; b8 b0 00
    jmp short 01e51h                          ; eb 03
    mov ax, 000a0h                            ; b8 a0 00
    movzx dx, byte [bp-018h]                  ; 0f b6 56 e8
    or ax, dx                                 ; 09 d0
    lea dx, [bx+006h]                         ; 8d 57 06
    out DX, AL                                ; ee
    lea dx, [bx+007h]                         ; 8d 57 07
    mov al, byte [bp-016h]                    ; 8a 46 ea
    out DX, AL                                ; ee
    mov ax, word [bp-016h]                    ; 8b 46 ea
    cmp ax, 000c4h                            ; 3d c4 00
    je short 01e6fh                           ; 74 05
    cmp ax, strict word 00029h                ; 3d 29 00
    jne short 01e7ch                          ; 75 0d
    mov ax, word [bp-014h]                    ; 8b 46 ec
    mov word [bp-01ah], ax                    ; 89 46 e6
    mov word [bp-014h], strict word 00001h    ; c7 46 ec 01 00
    jmp short 01e81h                          ; eb 05
    mov word [bp-01ah], strict word 00001h    ; c7 46 e6 01 00
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dl, al                                ; 88 c2
    test AL, strict byte 080h                 ; a8 80
    jne short 01e81h                          ; 75 f4
    test AL, strict byte 001h                 ; a8 01
    je short 01ea0h                           ; 74 0f
    mov dx, word [bp-01ch]                    ; 8b 56 e4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00002h                ; ba 02 00
    jmp near 01f39h                           ; e9 99 00
    test dl, 008h                             ; f6 c2 08
    jne short 01eb4h                          ; 75 0f
    mov dx, word [bp-01ch]                    ; 8b 56 e4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00003h                ; ba 03 00
    jmp near 01f39h                           ; e9 85 00
    sti                                       ; fb
    cmp di, 0f800h                            ; 81 ff 00 f8
    jc short 01ec8h                           ; 72 0d
    sub di, 00800h                            ; 81 ef 00 08
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    add ax, 00080h                            ; 05 80 00
    mov word [bp-00eh], ax                    ; 89 46 f2
    cmp byte [bp-006h], 001h                  ; 80 7e fa 01
    jne short 01edbh                          ; 75 0d
    mov dx, bx                                ; 89 da
    mov cx, word [bp-00ch]                    ; 8b 4e f4
    mov es, [bp-00eh]                         ; 8e 46 f2
    db  0f3h, 066h, 06dh
    ; rep insd                                  ; f3 66 6d
    jmp short 01ee5h                          ; eb 0a
    mov dx, bx                                ; 89 da
    mov cx, word [bp-00ch]                    ; 8b 4e f4
    mov es, [bp-00eh]                         ; 8e 46 f2
    rep insw                                  ; f3 6d
    mov ax, word [bp-01ah]                    ; 8b 46 e6
    mov es, [bp-00ah]                         ; 8e 46 f6
    add word [es:si+014h], ax                 ; 26 01 44 14
    dec word [bp-014h]                        ; ff 4e ec
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dl, al                                ; 88 c2
    test AL, strict byte 080h                 ; a8 80
    jne short 01ef2h                          ; 75 f4
    cmp word [bp-014h], strict byte 00000h    ; 83 7e ec 00
    jne short 01f18h                          ; 75 14
    and AL, strict byte 0c9h                  ; 24 c9
    cmp AL, strict byte 040h                  ; 3c 40
    je short 01f2eh                           ; 74 24
    mov dx, word [bp-01ch]                    ; 8b 56 e4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00004h                ; ba 04 00
    jmp short 01f39h                          ; eb 21
    mov al, dl                                ; 88 d0
    and AL, strict byte 0c9h                  ; 24 c9
    cmp AL, strict byte 048h                  ; 3c 48
    je short 01eb5h                           ; 74 95
    mov dx, word [bp-01ch]                    ; 8b 56 e4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00005h                ; ba 05 00
    jmp short 01f39h                          ; eb 0b
    mov dx, word [bp-01ch]                    ; 8b 56 e4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    xor dx, dx                                ; 31 d2
    mov ax, dx                                ; 89 d0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    jnle short 01f65h                         ; 7f 21
    mov BH, strict byte 021h                  ; b7 21
    mov bx, 0bf21h                            ; bb 21 bf
    and bx, ax                                ; 21 c3
    and di, ax                                ; 21 c7
    and bx, cx                                ; 21 cb
    and di, cx                                ; 21 cf
    db  021h
_ata_detect:                                 ; 0xf1f52 LB 0x627
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, 0025ah                            ; 81 ec 5a 02
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 b8 f6
    mov word [bp-026h], ax                    ; 89 46 da
    mov bx, 00122h                            ; bb 22 01
    mov es, ax                                ; 8e c0
    mov si, bx                                ; 89 de
    mov word [bp-02ah], ax                    ; 89 46 d6
    mov byte [es:bx+001c0h], 000h             ; 26 c6 87 c0 01 00
    db  066h, 026h, 0c7h, 087h, 0c2h, 001h, 0f0h, 001h, 0f0h, 003h
    ; mov dword [es:bx+001c2h], strict dword 003f001f0h ; 66 26 c7 87 c2 01 f0 01 f0 03
    mov byte [es:bx+001c1h], 00eh             ; 26 c6 87 c1 01 0e
    mov byte [es:bx+001c6h], 000h             ; 26 c6 87 c6 01 00
    db  066h, 026h, 0c7h, 087h, 0c8h, 001h, 070h, 001h, 070h, 003h
    ; mov dword [es:bx+001c8h], strict dword 003700170h ; 66 26 c7 87 c8 01 70 01 70 03
    mov byte [es:bx+001c7h], 00fh             ; 26 c6 87 c7 01 0f
    xor al, al                                ; 30 c0
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov byte [bp-018h], al                    ; 88 46 e8
    mov byte [bp-01ah], al                    ; 88 46 e6
    jmp near 02501h                           ; e9 56 05
    mov ax, 000a0h                            ; b8 a0 00
    lea dx, [bx+006h]                         ; 8d 57 06
    out DX, AL                                ; ee
    lea di, [bx+002h]                         ; 8d 7f 02
    mov AL, strict byte 055h                  ; b0 55
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    lea cx, [bx+003h]                         ; 8d 4f 03
    mov AL, strict byte 0aah                  ; b0 aa
    mov dx, cx                                ; 89 ca
    out DX, AL                                ; ee
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    mov AL, strict byte 055h                  ; b0 55
    mov dx, cx                                ; 89 ca
    out DX, AL                                ; ee
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    mov AL, strict byte 0aah                  ; b0 aa
    mov dx, cx                                ; 89 ca
    out DX, AL                                ; ee
    mov dx, di                                ; 89 fa
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-02ch], ax                    ; 89 46 d4
    mov dx, cx                                ; 89 ca
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp byte [bp-02ch], 055h                  ; 80 7e d4 55
    jne near 020b6h                           ; 0f 85 cf 00
    cmp AL, strict byte 0aah                  ; 3c aa
    jne near 020b6h                           ; 0f 85 c9 00
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov di, si                                ; 89 f7
    add di, ax                                ; 01 c7
    mov byte [es:di+01eh], 001h               ; 26 c6 45 1e 01
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    call 01c0ch                               ; e8 05 fc
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    je short 02012h                           ; 74 05
    mov ax, 000b0h                            ; b8 b0 00
    jmp short 02015h                          ; eb 03
    mov ax, 000a0h                            ; b8 a0 00
    lea dx, [bx+006h]                         ; 8d 57 06
    out DX, AL                                ; ee
    lea dx, [bx+002h]                         ; 8d 57 02
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov cx, ax                                ; 89 c1
    lea dx, [bx+003h]                         ; 8d 57 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp cl, 001h                              ; 80 f9 01
    jne near 020b6h                           ; 0f 85 88 00
    cmp al, cl                                ; 38 c8
    jne near 020b6h                           ; 0f 85 82 00
    lea dx, [bx+004h]                         ; 8d 57 04
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-02ch], ax                    ; 89 46 d4
    mov al, byte [bp-02ch]                    ; 8a 46 d4
    mov byte [bp-010h], al                    ; 88 46 f0
    lea dx, [bx+005h]                         ; 8d 57 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov cx, ax                                ; 89 c1
    mov byte [bp-00eh], al                    ; 88 46 f2
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp byte [bp-02ch], 014h                  ; 80 7e d4 14
    jne short 02072h                          ; 75 18
    cmp cl, 0ebh                              ; 80 f9 eb
    jne short 02072h                          ; 75 13
    movzx bx, byte [bp-01ah]                  ; 0f b6 5e e6
    imul bx, bx, strict byte 00018h           ; 6b db 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    add bx, si                                ; 01 f3
    mov byte [es:bx+01eh], 003h               ; 26 c6 47 1e 03
    jmp short 020b6h                          ; eb 44
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 02097h                          ; 75 1f
    cmp byte [bp-00eh], 000h                  ; 80 7e f2 00
    jne short 02097h                          ; 75 19
    test al, al                               ; 84 c0
    je short 02097h                           ; 74 15
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov byte [es:bx+01eh], 002h               ; 26 c6 47 1e 02
    jmp short 020b6h                          ; eb 1f
    cmp byte [bp-010h], 0ffh                  ; 80 7e f0 ff
    jne short 020b6h                          ; 75 19
    cmp byte [bp-00eh], 0ffh                  ; 80 7e f2 ff
    jne short 020b6h                          ; 75 13
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov byte [es:bx+01eh], 000h               ; 26 c6 47 1e 00
    mov dx, word [bp-030h]                    ; 8b 56 d0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov al, byte [es:bx+01eh]                 ; 26 8a 47 1e
    mov byte [bp-006h], al                    ; 88 46 fa
    cmp AL, strict byte 002h                  ; 3c 02
    jne near 022d5h                           ; 0f 85 fb 01
    mov byte [es:bx+01fh], 0ffh               ; 26 c6 47 1f ff
    mov byte [es:bx+022h], 000h               ; 26 c6 47 22 00
    lea dx, [bp-0025eh]                       ; 8d 96 a2 fd
    mov word [es:si+004h], dx                 ; 26 89 54 04
    mov [es:si+006h], ss                      ; 26 8c 54 06
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    mov byte [es:si+008h], al                 ; 26 88 44 08
    mov cx, strict word 00001h                ; b9 01 00
    mov bx, 000ech                            ; bb ec 00
    mov ax, si                                ; 89 f0
    mov dx, es                                ; 8c c2
    call 01ceah                               ; e8 e6 fb
    test ax, ax                               ; 85 c0
    je short 02113h                           ; 74 0b
    push 00084h                               ; 68 84 00
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 fd f7
    add sp, strict byte 00004h                ; 83 c4 04
    test byte [bp-0025eh], 080h               ; f6 86 a2 fd 80
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    mov byte [bp-00ch], al                    ; 88 46 f4
    cmp byte [bp-001feh], 000h                ; 80 be 02 fe 00
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    mov byte [bp-012h], al                    ; 88 46 ee
    mov word [bp-028h], 00200h                ; c7 46 d8 00 02
    mov ax, word [bp-0025ch]                  ; 8b 86 a4 fd
    mov word [bp-022h], ax                    ; 89 46 de
    mov ax, word [bp-00258h]                  ; 8b 86 a8 fd
    mov word [bp-020h], ax                    ; 89 46 e0
    mov ax, word [bp-00252h]                  ; 8b 86 ae fd
    mov word [bp-01ch], ax                    ; 89 46 e4
    mov ax, word [bp-001e6h]                  ; 8b 86 1a fe
    mov word [bp-024h], ax                    ; 89 46 dc
    mov ax, word [bp-001e4h]                  ; 8b 86 1c fe
    mov word [bp-02eh], ax                    ; 89 46 d2
    cmp ax, 00fffh                            ; 3d ff 0f
    jne short 0216eh                          ; 75 14
    cmp word [bp-024h], strict byte 0ffffh    ; 83 7e dc ff
    jne short 0216eh                          ; 75 0e
    mov ax, word [bp-00196h]                  ; 8b 86 6a fe
    mov word [bp-024h], ax                    ; 89 46 dc
    mov ax, word [bp-00194h]                  ; 8b 86 6c fe
    mov word [bp-02eh], ax                    ; 89 46 d2
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    cmp AL, strict byte 007h                  ; 3c 07
    jnbe short 021d3h                         ; 77 5e
    movzx bx, al                              ; 0f b6 d8
    add bx, bx                                ; 01 db
    jmp word [cs:bx+01f42h]                   ; 2e ff a7 42 1f
    mov BL, strict byte 01eh                  ; b3 1e
    mov al, bl                                ; 88 d8
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    xor ah, ah                                ; 30 e4
    call 0165ch                               ; e8 d2 f4
    xor ah, ah                                ; 30 e4
    mov di, ax                                ; 89 c7
    sal di, 008h                              ; c1 e7 08
    movzx ax, bl                              ; 0f b6 c3
    call 0165ch                               ; e8 c5 f4
    xor ah, ah                                ; 30 e4
    add di, ax                                ; 01 c7
    mov al, bl                                ; 88 d8
    add AL, strict byte 002h                  ; 04 02
    xor ah, ah                                ; 30 e4
    call 0165ch                               ; e8 b8 f4
    movzx dx, al                              ; 0f b6 d0
    mov al, bl                                ; 88 d8
    add AL, strict byte 007h                  ; 04 07
    xor ah, ah                                ; 30 e4
    call 0165ch                               ; e8 ac f4
    xor ah, ah                                ; 30 e4
    mov word [bp-01eh], ax                    ; 89 46 e2
    jmp short 021dah                          ; eb 23
    mov BL, strict byte 026h                  ; b3 26
    jmp short 02181h                          ; eb c6
    mov BL, strict byte 067h                  ; b3 67
    jmp short 02181h                          ; eb c2
    mov BL, strict byte 070h                  ; b3 70
    jmp short 02181h                          ; eb be
    mov BL, strict byte 040h                  ; b3 40
    jmp short 02181h                          ; eb ba
    mov BL, strict byte 048h                  ; b3 48
    jmp short 02181h                          ; eb b6
    mov BL, strict byte 050h                  ; b3 50
    jmp short 02181h                          ; eb b2
    mov BL, strict byte 058h                  ; b3 58
    jmp short 02181h                          ; eb ae
    xor di, di                                ; 31 ff
    xor dx, dx                                ; 31 d2
    mov word [bp-01eh], di                    ; 89 7e e2
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 e7 f6
    push word [bp-01eh]                       ; ff 76 e2
    push dx                                   ; 52
    push di                                   ; 57
    push word [bp-01ch]                       ; ff 76 e4
    push dword [bp-022h]                      ; 66 ff 76 de
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    push 000adh                               ; 68 ad 00
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 0a f7
    add sp, strict byte 00014h                ; 83 c4 14
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov byte [es:bx+01fh], 0ffh               ; 26 c6 47 1f ff
    mov al, byte [bp-00ch]                    ; 8a 46 f4
    mov byte [es:bx+020h], al                 ; 26 88 47 20
    mov al, byte [bp-012h]                    ; 8a 46 ee
    mov byte [es:bx+022h], al                 ; 26 88 47 22
    mov ax, word [bp-028h]                    ; 8b 46 d8
    mov word [es:bx+024h], ax                 ; 26 89 47 24
    mov ax, word [bp-020h]                    ; 8b 46 e0
    mov word [es:bx+02ch], ax                 ; 26 89 47 2c
    mov ax, word [bp-022h]                    ; 8b 46 de
    mov word [es:bx+02eh], ax                 ; 26 89 47 2e
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    mov word [es:bx+030h], ax                 ; 26 89 47 30
    mov ax, word [bp-024h]                    ; 8b 46 dc
    mov word [es:bx+032h], ax                 ; 26 89 47 32
    mov ax, word [bp-02eh]                    ; 8b 46 d2
    mov word [es:bx+034h], ax                 ; 26 89 47 34
    mov word [es:bx+026h], dx                 ; 26 89 57 26
    mov word [es:bx+028h], di                 ; 26 89 7f 28
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    mov word [es:bx+02ah], ax                 ; 26 89 47 2a
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    cmp AL, strict byte 002h                  ; 3c 02
    jnc short 022c1h                          ; 73 5a
    test al, al                               ; 84 c0
    jne short 02270h                          ; 75 05
    mov bx, strict word 0003dh                ; bb 3d 00
    jmp short 02273h                          ; eb 03
    mov bx, strict word 0004dh                ; bb 4d 00
    mov cx, word [bp-026h]                    ; 8b 4e da
    mov es, cx                                ; 8e c1
    mov word [es:bx], di                      ; 26 89 3f
    mov byte [es:bx+002h], dl                 ; 26 88 57 02
    mov byte [es:bx+003h], 0a0h               ; 26 c6 47 03 a0
    mov al, byte [bp-01ch]                    ; 8a 46 e4
    mov byte [es:bx+004h], al                 ; 26 88 47 04
    mov ax, word [bp-022h]                    ; 8b 46 de
    mov word [es:bx+009h], ax                 ; 26 89 47 09
    mov al, byte [bp-020h]                    ; 8a 46 e0
    mov byte [es:bx+00bh], al                 ; 26 88 47 0b
    mov al, byte [bp-01ch]                    ; 8a 46 e4
    mov byte [es:bx+00eh], al                 ; 26 88 47 0e
    xor al, al                                ; 30 c0
    xor ah, ah                                ; 30 e4
    jmp short 022abh                          ; eb 05
    cmp ah, 00fh                              ; 80 fc 0f
    jnc short 022b9h                          ; 73 0e
    movzx di, ah                              ; 0f b6 fc
    mov es, cx                                ; 8e c1
    add di, bx                                ; 01 df
    add al, byte [es:di]                      ; 26 02 05
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    jmp short 022a6h                          ; eb ed
    neg al                                    ; f6 d8
    mov es, cx                                ; 8e c1
    mov byte [es:bx+00fh], al                 ; 26 88 47 0f
    movzx bx, byte [bp-018h]                  ; 0f b6 5e e8
    mov es, [bp-02ah]                         ; 8e 46 d6
    add bx, si                                ; 01 f3
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    mov byte [es:bx+0019fh], al               ; 26 88 87 9f 01
    inc byte [bp-018h]                        ; fe 46 e8
    cmp byte [bp-006h], 003h                  ; 80 7e fa 03
    jne near 02370h                           ; 0f 85 93 00
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov byte [es:bx+01fh], 005h               ; 26 c6 47 1f 05
    mov byte [es:bx+022h], 000h               ; 26 c6 47 22 00
    lea dx, [bp-0025eh]                       ; 8d 96 a2 fd
    mov word [es:si+004h], dx                 ; 26 89 54 04
    mov [es:si+006h], ss                      ; 26 8c 54 06
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    mov byte [es:si+008h], al                 ; 26 88 44 08
    mov cx, strict word 00001h                ; b9 01 00
    mov bx, 000a1h                            ; bb a1 00
    mov ax, si                                ; 89 f0
    mov dx, es                                ; 8c c2
    call 01ceah                               ; e8 d5 f9
    test ax, ax                               ; 85 c0
    je short 02324h                           ; 74 0b
    push 000d4h                               ; 68 d4 00
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 ec f5
    add sp, strict byte 00004h                ; 83 c4 04
    mov dl, byte [bp-0025dh]                  ; 8a 96 a3 fd
    and dl, 01fh                              ; 80 e2 1f
    test byte [bp-0025eh], 080h               ; f6 86 a2 fd 80
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    mov cx, ax                                ; 89 c1
    cmp byte [bp-001feh], 000h                ; 80 be 02 fe 00
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    movzx bx, byte [bp-01ah]                  ; 0f b6 5e e6
    imul bx, bx, strict byte 00018h           ; 6b db 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    add bx, si                                ; 01 f3
    mov byte [es:bx+01fh], dl                 ; 26 88 57 1f
    mov byte [es:bx+020h], cl                 ; 26 88 4f 20
    mov byte [es:bx+022h], al                 ; 26 88 47 22
    mov word [es:bx+024h], 00800h             ; 26 c7 47 24 00 08
    movzx bx, byte [bp-00ah]                  ; 0f b6 5e f6
    add bx, si                                ; 01 f3
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    mov byte [es:bx+001b0h], al               ; 26 88 87 b0 01
    inc byte [bp-00ah]                        ; fe 46 f6
    mov al, byte [bp-006h]                    ; 8a 46 fa
    cmp AL, strict byte 003h                  ; 3c 03
    je short 023a4h                           ; 74 2d
    cmp AL, strict byte 002h                  ; 3c 02
    jne near 02409h                           ; 0f 85 8c 00
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov ax, word [es:bx+032h]                 ; 26 8b 47 32
    mov word [bp-034h], ax                    ; 89 46 cc
    mov ax, word [es:bx+034h]                 ; 26 8b 47 34
    mov word [bp-032h], ax                    ; 89 46 ce
    mov cx, strict word 0000bh                ; b9 0b 00
    shr word [bp-032h], 1                     ; d1 6e ce
    rcr word [bp-034h], 1                     ; d1 5e cc
    loop 0239ch                               ; e2 f8
    movzx dx, byte [bp-001bdh]                ; 0f b6 96 43 fe
    sal dx, 008h                              ; c1 e2 08
    movzx ax, byte [bp-001beh]                ; 0f b6 86 42 fe
    or dx, ax                                 ; 09 c2
    mov byte [bp-014h], 00fh                  ; c6 46 ec 0f
    jmp short 023c2h                          ; eb 09
    dec byte [bp-014h]                        ; fe 4e ec
    cmp byte [bp-014h], 000h                  ; 80 7e ec 00
    jbe short 023cfh                          ; 76 0d
    movzx cx, byte [bp-014h]                  ; 0f b6 4e ec
    mov ax, strict word 00001h                ; b8 01 00
    sal ax, CL                                ; d3 e0
    test dx, ax                               ; 85 c2
    je short 023b9h                           ; 74 ea
    xor bx, bx                                ; 31 db
    jmp short 023d8h                          ; eb 05
    cmp bx, strict byte 00014h                ; 83 fb 14
    jnl short 023edh                          ; 7d 15
    mov di, bx                                ; 89 df
    add di, bx                                ; 01 df
    mov al, byte [bp+di-00227h]               ; 8a 83 d9 fd
    mov byte [bp+di-05eh], al                 ; 88 43 a2
    mov al, byte [bp+di-00228h]               ; 8a 83 d8 fd
    mov byte [bp+di-05dh], al                 ; 88 43 a3
    inc bx                                    ; 43
    jmp short 023d3h                          ; eb e6
    mov byte [bp-036h], 000h                  ; c6 46 ca 00
    mov bx, strict word 00027h                ; bb 27 00
    jmp short 023fbh                          ; eb 05
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jle short 02409h                          ; 7e 0e
    mov di, bx                                ; 89 df
    cmp byte [bp+di-05eh], 020h               ; 80 7b a2 20
    jne short 02409h                          ; 75 06
    mov byte [bp+di-05eh], 000h               ; c6 43 a2 00
    jmp short 023f6h                          ; eb ed
    mov al, byte [bp-006h]                    ; 8a 46 fa
    cmp AL, strict byte 003h                  ; 3c 03
    je short 0246eh                           ; 74 5e
    cmp AL, strict byte 002h                  ; 3c 02
    je short 0241dh                           ; 74 09
    cmp AL, strict byte 001h                  ; 3c 01
    je near 024d9h                            ; 0f 84 bf 00
    jmp near 024f8h                           ; e9 db 00
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    je short 02428h                           ; 74 05
    mov ax, 000ffh                            ; b8 ff 00
    jmp short 0242bh                          ; eb 03
    mov ax, 00106h                            ; b8 06 01
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    push 0010dh                               ; 68 0d 01
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 d4 f4
    add sp, strict byte 00008h                ; 83 c4 08
    xor bx, bx                                ; 31 db
    mov di, bx                                ; 89 df
    movzx ax, byte [bp+di-05eh]               ; 0f b6 43 a2
    inc bx                                    ; 43
    test ax, ax                               ; 85 c0
    je short 02457h                           ; 74 0e
    push ax                                   ; 50
    push 00118h                               ; 68 18 01
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 bb f4
    add sp, strict byte 00006h                ; 83 c4 06
    jmp short 0243eh                          ; eb e7
    push dword [bp-034h]                      ; 66 ff 76 cc
    movzx ax, byte [bp-014h]                  ; 0f b6 46 ec
    push ax                                   ; 50
    push 0011bh                               ; 68 1b 01
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 a5 f4
    add sp, strict byte 0000ah                ; 83 c4 0a
    jmp near 024f8h                           ; e9 8a 00
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    je short 02479h                           ; 74 05
    mov ax, 000ffh                            ; b8 ff 00
    jmp short 0247ch                          ; eb 03
    mov ax, 00106h                            ; b8 06 01
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    push 0010dh                               ; 68 0d 01
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 83 f4
    add sp, strict byte 00008h                ; 83 c4 08
    xor bx, bx                                ; 31 db
    mov di, bx                                ; 89 df
    movzx ax, byte [bp+di-05eh]               ; 0f b6 43 a2
    inc bx                                    ; 43
    test ax, ax                               ; 85 c0
    je short 024a8h                           ; 74 0e
    push ax                                   ; 50
    push 00118h                               ; 68 18 01
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 6a f4
    add sp, strict byte 00006h                ; 83 c4 06
    jmp short 0248fh                          ; eb e7
    movzx ax, byte [bp-01ah]                  ; 0f b6 46 e6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    cmp byte [es:bx+01fh], 005h               ; 26 80 7f 1f 05
    jne short 024c7h                          ; 75 0a
    movzx ax, byte [bp-014h]                  ; 0f b6 46 ec
    push ax                                   ; 50
    push 0013bh                               ; 68 3b 01
    jmp short 024cfh                          ; eb 08
    movzx ax, byte [bp-014h]                  ; 0f b6 46 ec
    push ax                                   ; 50
    push 00155h                               ; 68 55 01
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 39 f4
    add sp, strict byte 00006h                ; 83 c4 06
    jmp short 024f8h                          ; eb 1f
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    je short 024e4h                           ; 74 05
    mov ax, 000ffh                            ; b8 ff 00
    jmp short 024e7h                          ; eb 03
    mov ax, 00106h                            ; b8 06 01
    push ax                                   ; 50
    movzx ax, byte [bp-016h]                  ; 0f b6 46 ea
    push ax                                   ; 50
    push 00167h                               ; 68 67 01
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 18 f4
    add sp, strict byte 00008h                ; 83 c4 08
    inc byte [bp-01ah]                        ; fe 46 e6
    cmp byte [bp-01ah], 008h                  ; 80 7e e6 08
    jnc short 02552h                          ; 73 51
    movzx bx, byte [bp-01ah]                  ; 0f b6 5e e6
    mov ax, bx                                ; 89 d8
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    mov cx, ax                                ; 89 c1
    mov byte [bp-016h], al                    ; 88 46 ea
    mov ax, bx                                ; 89 d8
    cwd                                       ; 99
    mov bx, strict word 00002h                ; bb 02 00
    idiv bx                                   ; f7 fb
    mov word [bp-02ch], dx                    ; 89 56 d4
    mov al, byte [bp-02ch]                    ; 8a 46 d4
    mov byte [bp-008h], al                    ; 88 46 f8
    movzx ax, cl                              ; 0f b6 c1
    imul ax, ax, strict byte 00006h           ; 6b c0 06
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov di, si                                ; 89 f7
    add di, ax                                ; 01 c7
    mov bx, word [es:di+001c2h]               ; 26 8b 9d c2 01
    mov ax, word [es:di+001c4h]               ; 26 8b 85 c4 01
    mov word [bp-030h], ax                    ; 89 46 d0
    mov dx, ax                                ; 89 c2
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 00ah                  ; b0 0a
    out DX, AL                                ; ee
    cmp byte [bp-02ch], 000h                  ; 80 7e d4 00
    je near 01fabh                            ; 0f 84 5f fa
    mov ax, 000b0h                            ; b8 b0 00
    jmp near 01faeh                           ; e9 5c fa
    mov al, byte [bp-018h]                    ; 8a 46 e8
    mov es, [bp-02ah]                         ; 8e 46 d6
    mov byte [es:si+0019eh], al               ; 26 88 84 9e 01
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    mov byte [es:si+001afh], al               ; 26 88 84 af 01
    movzx bx, byte [bp-018h]                  ; 0f b6 5e e8
    mov dx, strict word 00075h                ; ba 75 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 9c f0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
ata_cmd_data_out_:                           ; 0xf2579 LB 0x21a
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0001ah                ; 83 ec 1a
    mov di, ax                                ; 89 c7
    mov word [bp-00ah], dx                    ; 89 56 f6
    mov word [bp-01ah], bx                    ; 89 5e e6
    mov word [bp-00eh], cx                    ; 89 4e f2
    mov es, dx                                ; 8e c2
    movzx ax, byte [es:di+008h]               ; 26 0f b6 45 08
    mov dx, ax                                ; 89 c2
    shr dx, 1                                 ; d1 ea
    mov dh, al                                ; 88 c6
    and dh, 001h                              ; 80 e6 01
    mov byte [bp-006h], dh                    ; 88 76 fa
    movzx si, dl                              ; 0f b6 f2
    imul si, si, strict byte 00006h           ; 6b f6 06
    add si, di                                ; 01 fe
    mov bx, word [es:si+001c2h]               ; 26 8b 9c c2 01
    mov dx, word [es:si+001c4h]               ; 26 8b 94 c4 01
    mov word [bp-00ch], dx                    ; 89 56 f4
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov si, di                                ; 89 fe
    add si, ax                                ; 01 c6
    mov al, byte [es:si+022h]                 ; 26 8a 44 22
    mov byte [bp-008h], al                    ; 88 46 f8
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 025cdh                          ; 75 07
    mov word [bp-012h], 00080h                ; c7 46 ee 80 00
    jmp short 025d2h                          ; eb 05
    mov word [bp-012h], 00100h                ; c7 46 ee 00 01
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 080h                 ; a8 80
    je short 025ebh                           ; 74 0f
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00001h                ; ba 01 00
    jmp near 0278ah                           ; e9 9f 01
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov ax, word [es:di]                      ; 26 8b 05
    mov word [bp-01eh], ax                    ; 89 46 e2
    mov ax, word [es:di+002h]                 ; 26 8b 45 02
    mov word [bp-01ch], ax                    ; 89 46 e4
    mov si, word [es:di+004h]                 ; 26 8b 75 04
    mov ax, word [es:di+006h]                 ; 26 8b 45 06
    mov word [bp-018h], ax                    ; 89 46 e8
    mov ax, word [es:di+012h]                 ; 26 8b 45 12
    mov word [bp-010h], ax                    ; 89 46 f0
    mov ax, word [es:di+00eh]                 ; 26 8b 45 0e
    mov word [bp-016h], ax                    ; 89 46 ea
    mov ax, word [es:di+010h]                 ; 26 8b 45 10
    mov word [bp-014h], ax                    ; 89 46 ec
    mov ax, word [bp-010h]                    ; 8b 46 f0
    test ax, ax                               ; 85 c0
    jne short 02689h                          ; 75 67
    mov dx, word [bp-01eh]                    ; 8b 56 e2
    add dx, word [bp-00eh]                    ; 03 56 f2
    adc ax, word [bp-01ch]                    ; 13 46 e4
    cmp ax, 01000h                            ; 3d 00 10
    jnbe short 02632h                         ; 77 02
    jne short 0265dh                          ; 75 2b
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov word [bp-010h], ax                    ; 89 46 f0
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    lea dx, [bx+002h]                         ; 8d 57 02
    out DX, AL                                ; ee
    lea dx, [bx+003h]                         ; 8d 57 03
    mov al, byte [bp-010h]                    ; 8a 46 f0
    out DX, AL                                ; ee
    lea dx, [bx+004h]                         ; 8d 57 04
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    lea dx, [bx+005h]                         ; 8d 57 05
    out DX, AL                                ; ee
    mov byte [bp-01bh], al                    ; 88 46 e5
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    xor ah, ah                                ; 30 e4
    mov word [bp-010h], ax                    ; 89 46 f0
    mov cx, strict word 00008h                ; b9 08 00
    shr word [bp-01ch], 1                     ; d1 6e e4
    rcr word [bp-01eh], 1                     ; d1 5e e2
    loop 02668h                               ; e2 f8
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    mov word [bp-016h], ax                    ; 89 46 ea
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    mov word [bp-01eh], ax                    ; 89 46 e2
    mov word [bp-01ch], strict word 00000h    ; c7 46 e4 00 00
    and ax, strict word 0000fh                ; 25 0f 00
    or AL, strict byte 040h                   ; 0c 40
    mov word [bp-014h], ax                    ; 89 46 ec
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 00ah                  ; b0 0a
    out DX, AL                                ; ee
    lea dx, [bx+001h]                         ; 8d 57 01
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    lea dx, [bx+002h]                         ; 8d 57 02
    mov al, byte [bp-00eh]                    ; 8a 46 f2
    out DX, AL                                ; ee
    lea dx, [bx+003h]                         ; 8d 57 03
    mov al, byte [bp-010h]                    ; 8a 46 f0
    out DX, AL                                ; ee
    mov ax, word [bp-016h]                    ; 8b 46 ea
    lea dx, [bx+004h]                         ; 8d 57 04
    out DX, AL                                ; ee
    shr ax, 008h                              ; c1 e8 08
    lea dx, [bx+005h]                         ; 8d 57 05
    out DX, AL                                ; ee
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    je short 026bfh                           ; 74 05
    mov ax, 000b0h                            ; b8 b0 00
    jmp short 026c2h                          ; eb 03
    mov ax, 000a0h                            ; b8 a0 00
    movzx dx, byte [bp-014h]                  ; 0f b6 56 ec
    or ax, dx                                 ; 09 d0
    lea dx, [bx+006h]                         ; 8d 57 06
    out DX, AL                                ; ee
    lea dx, [bx+007h]                         ; 8d 57 07
    mov al, byte [bp-01ah]                    ; 8a 46 e6
    out DX, AL                                ; ee
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dl, al                                ; 88 c2
    test AL, strict byte 080h                 ; a8 80
    jne short 026d3h                          ; 75 f4
    test AL, strict byte 001h                 ; a8 01
    je short 026f2h                           ; 74 0f
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00002h                ; ba 02 00
    jmp near 0278ah                           ; e9 98 00
    test dl, 008h                             ; f6 c2 08
    jne short 02706h                          ; 75 0f
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00003h                ; ba 03 00
    jmp near 0278ah                           ; e9 84 00
    sti                                       ; fb
    cmp si, 0f800h                            ; 81 fe 00 f8
    jc short 0271ah                           ; 72 0d
    sub si, 00800h                            ; 81 ee 00 08
    mov ax, word [bp-018h]                    ; 8b 46 e8
    add ax, 00080h                            ; 05 80 00
    mov word [bp-018h], ax                    ; 89 46 e8
    cmp byte [bp-008h], 001h                  ; 80 7e f8 01
    jne short 0272eh                          ; 75 0e
    mov dx, bx                                ; 89 da
    mov cx, word [bp-012h]                    ; 8b 4e ee
    mov es, [bp-018h]                         ; 8e 46 e8
    db  0f3h, 066h, 026h, 06fh
    ; rep es outsd                              ; f3 66 26 6f
    jmp short 02739h                          ; eb 0b
    mov dx, bx                                ; 89 da
    mov cx, word [bp-012h]                    ; 8b 4e ee
    mov es, [bp-018h]                         ; 8e 46 e8
    db  0f3h, 026h, 06fh
    ; rep es outsw                              ; f3 26 6f
    mov es, [bp-00ah]                         ; 8e 46 f6
    inc word [es:di+014h]                     ; 26 ff 45 14
    dec word [bp-00eh]                        ; ff 4e f2
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dl, al                                ; 88 c2
    test AL, strict byte 080h                 ; a8 80
    jne short 02743h                          ; 75 f4
    cmp word [bp-00eh], strict byte 00000h    ; 83 7e f2 00
    jne short 02769h                          ; 75 14
    and AL, strict byte 0e9h                  ; 24 e9
    cmp AL, strict byte 040h                  ; 3c 40
    je short 0277fh                           ; 74 24
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00006h                ; ba 06 00
    jmp short 0278ah                          ; eb 21
    mov al, dl                                ; 88 d0
    and AL, strict byte 0c9h                  ; 24 c9
    cmp AL, strict byte 048h                  ; 3c 48
    je short 02707h                           ; 74 96
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00007h                ; ba 07 00
    jmp short 0278ah                          ; eb 0b
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    xor dx, dx                                ; 31 d2
    mov ax, dx                                ; 89 d0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
@ata_read_sectors:                           ; 0xf2793 LB 0x7a
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov si, word [bp+004h]                    ; 8b 76 04
    mov es, [bp+006h]                         ; 8e 46 06
    mov bl, byte [es:si+008h]                 ; 26 8a 5c 08
    mov cx, word [es:si+00ah]                 ; 26 8b 4c 0a
    mov dx, cx                                ; 89 ca
    sal dx, 009h                              ; c1 e2 09
    mov ax, word [es:si+012h]                 ; 26 8b 44 12
    test ax, ax                               ; 85 c0
    je short 027c1h                           ; 74 0d
    movzx di, bl                              ; 0f b6 fb
    imul di, di, strict byte 00018h           ; 6b ff 18
    mov [bp-006h], es                         ; 8c 46 fa
    add di, si                                ; 01 f7
    jmp short 027edh                          ; eb 2c
    mov di, word [es:si]                      ; 26 8b 3c
    add di, cx                                ; 01 cf
    mov word [bp-006h], di                    ; 89 7e fa
    adc ax, word [es:si+002h]                 ; 26 13 44 02
    cmp ax, 01000h                            ; 3d 00 10
    jnbe short 027d4h                         ; 77 02
    jne short 027e0h                          ; 75 0c
    mov bx, strict word 00024h                ; bb 24 00
    mov ax, si                                ; 89 f0
    mov dx, es                                ; 8c c2
    call 01ceah                               ; e8 0c f5
    jmp short 02804h                          ; eb 24
    movzx ax, bl                              ; 0f b6 c3
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov [bp-006h], es                         ; 8c 46 fa
    mov di, si                                ; 89 f7
    add di, ax                                ; 01 c7
    mov word [es:di+024h], dx                 ; 26 89 55 24
    mov bx, 000c4h                            ; bb c4 00
    mov ax, si                                ; 89 f0
    mov dx, es                                ; 8c c2
    call 01ceah                               ; e8 ef f4
    mov es, [bp-006h]                         ; 8e 46 fa
    mov word [es:di+024h], 00200h             ; 26 c7 45 24 00 02
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
@ata_write_sectors:                          ; 0xf280d LB 0x3d
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    les si, [bp+004h]                         ; c4 76 04
    mov cx, word [es:si+00ah]                 ; 26 8b 4c 0a
    cmp word [es:si+012h], strict byte 00000h ; 26 83 7c 12 00
    je short 0282bh                           ; 74 0c
    mov bx, strict word 00030h                ; bb 30 00
    mov ax, si                                ; 89 f0
    mov dx, es                                ; 8c c2
    call 02579h                               ; e8 50 fd
    jmp short 02842h                          ; eb 17
    xor ax, ax                                ; 31 c0
    mov dx, word [es:si]                      ; 26 8b 14
    add dx, cx                                ; 01 ca
    adc ax, word [es:si+002h]                 ; 26 13 44 02
    cmp ax, 01000h                            ; 3d 00 10
    jnbe short 0283dh                         ; 77 02
    jne short 0281fh                          ; 75 e2
    mov bx, strict word 00034h                ; bb 34 00
    jmp short 02822h                          ; eb e0
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
ata_cmd_packet_:                             ; 0xf284a LB 0x2e8
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00014h                ; 83 ec 14
    push ax                                   ; 50
    mov byte [bp-008h], dl                    ; 88 56 f8
    mov di, bx                                ; 89 df
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 bb ed
    mov word [bp-012h], 00122h                ; c7 46 ee 22 01
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov ax, word [bp-01ah]                    ; 8b 46 e6
    shr ax, 1                                 ; d1 e8
    mov ah, byte [bp-01ah]                    ; 8a 66 e6
    and ah, 001h                              ; 80 e4 01
    mov byte [bp-006h], ah                    ; 88 66 fa
    cmp byte [bp+00ah], 002h                  ; 80 7e 0a 02
    jne short 0289ch                          ; 75 1f
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 44 f0
    push 00181h                               ; 68 81 01
    push 00190h                               ; 68 90 01
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 7a f0
    add sp, strict byte 00006h                ; 83 c4 06
    mov dx, strict word 00001h                ; ba 01 00
    jmp near 02b27h                           ; e9 8b 02
    test byte [bp+004h], 001h                 ; f6 46 04 01
    jne short 02896h                          ; 75 f4
    xor ah, ah                                ; 30 e4
    imul ax, ax, strict byte 00006h           ; 6b c0 06
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov si, word [bp-012h]                    ; 8b 76 ee
    add si, ax                                ; 01 c6
    mov bx, word [es:si+001c2h]               ; 26 8b 9c c2 01
    mov ax, word [es:si+001c4h]               ; 26 8b 84 c4 01
    mov word [bp-010h], ax                    ; 89 46 f0
    imul si, word [bp-01ah], strict byte 00018h ; 6b 76 e6 18
    add si, word [bp-012h]                    ; 03 76 ee
    mov al, byte [es:si+022h]                 ; 26 8a 44 22
    mov byte [bp-00ah], al                    ; 88 46 f6
    xor ax, ax                                ; 31 c0
    mov word [bp-018h], ax                    ; 89 46 e8
    mov word [bp-016h], ax                    ; 89 46 ea
    mov al, byte [bp-008h]                    ; 8a 46 f8
    cmp AL, strict byte 00ch                  ; 3c 0c
    jnc short 028dfh                          ; 73 06
    mov byte [bp-008h], 00ch                  ; c6 46 f8 0c
    jmp short 028e5h                          ; eb 06
    jbe short 028e5h                          ; 76 04
    mov byte [bp-008h], 010h                  ; c6 46 f8 10
    shr byte [bp-008h], 1                     ; d0 6e f8
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov si, word [bp-012h]                    ; 8b 76 ee
    db  066h, 026h, 0c7h, 044h, 014h, 000h, 000h, 000h, 000h
    ; mov dword [es:si+014h], strict dword 000000000h ; 66 26 c7 44 14 00 00 00 00
    mov word [es:si+018h], strict word 00000h ; 26 c7 44 18 00 00
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 080h                 ; a8 80
    je short 0290dh                           ; 74 06
    mov dx, strict word 00002h                ; ba 02 00
    jmp near 02b27h                           ; e9 1a 02
    mov dx, word [bp-010h]                    ; 8b 56 f0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 00ah                  ; b0 0a
    out DX, AL                                ; ee
    lea dx, [bx+004h]                         ; 8d 57 04
    mov AL, strict byte 0f0h                  ; b0 f0
    out DX, AL                                ; ee
    lea dx, [bx+005h]                         ; 8d 57 05
    mov AL, strict byte 0ffh                  ; b0 ff
    out DX, AL                                ; ee
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    je short 0292dh                           ; 74 05
    mov ax, 000b0h                            ; b8 b0 00
    jmp short 02930h                          ; eb 03
    mov ax, 000a0h                            ; b8 a0 00
    lea dx, [bx+006h]                         ; 8d 57 06
    out DX, AL                                ; ee
    lea dx, [bx+007h]                         ; 8d 57 07
    mov AL, strict byte 0a0h                  ; b0 a0
    out DX, AL                                ; ee
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dl, al                                ; 88 c2
    test AL, strict byte 080h                 ; a8 80
    jne short 0293ah                          ; 75 f4
    test AL, strict byte 001h                 ; a8 01
    je short 02959h                           ; 74 0f
    mov dx, word [bp-010h]                    ; 8b 56 f0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00003h                ; ba 03 00
    jmp near 02b27h                           ; e9 ce 01
    test dl, 008h                             ; f6 c2 08
    jne short 0296dh                          ; 75 0f
    mov dx, word [bp-010h]                    ; 8b 56 f0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, strict word 00004h                ; ba 04 00
    jmp near 02b27h                           ; e9 ba 01
    sti                                       ; fb
    mov ax, di                                ; 89 f8
    shr ax, 004h                              ; c1 e8 04
    add ax, cx                                ; 01 c8
    mov si, di                                ; 89 fe
    and si, strict byte 0000fh                ; 83 e6 0f
    movzx cx, byte [bp-008h]                  ; 0f b6 4e f8
    mov dx, bx                                ; 89 da
    mov es, ax                                ; 8e c0
    db  0f3h, 026h, 06fh
    ; rep es outsw                              ; f3 26 6f
    cmp byte [bp+00ah], 000h                  ; 80 7e 0a 00
    jne short 02996h                          ; 75 0b
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dl, al                                ; 88 c2
    jmp near 02b08h                           ; e9 72 01
    lea dx, [bx+007h]                         ; 8d 57 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov dl, al                                ; 88 c2
    test AL, strict byte 080h                 ; a8 80
    jne short 02996h                          ; 75 f4
    test AL, strict byte 088h                 ; a8 88
    je near 02b08h                            ; 0f 84 60 01
    test AL, strict byte 001h                 ; a8 01
    je short 029b7h                           ; 74 0b
    mov dx, word [bp-010h]                    ; 8b 56 f0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    jmp short 02953h                          ; eb 9c
    mov al, dl                                ; 88 d0
    and AL, strict byte 0c9h                  ; 24 c9
    cmp AL, strict byte 048h                  ; 3c 48
    je short 029cah                           ; 74 0b
    mov dx, word [bp-010h]                    ; 8b 56 f0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    jmp short 02967h                          ; eb 9d
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 004h                              ; c1 e8 04
    mov dx, word [bp+00eh]                    ; 8b 56 0e
    add dx, ax                                ; 01 c2
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    and ax, strict word 0000fh                ; 25 0f 00
    mov word [bp+00ch], ax                    ; 89 46 0c
    mov word [bp+00eh], dx                    ; 89 56 0e
    lea dx, [bx+005h]                         ; 8d 57 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov cx, ax                                ; 89 c1
    sal cx, 008h                              ; c1 e1 08
    lea dx, [bx+004h]                         ; 8d 57 04
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    add cx, ax                                ; 01 c1
    mov word [bp-014h], cx                    ; 89 4e ec
    mov ax, word [bp+004h]                    ; 8b 46 04
    cmp ax, cx                                ; 39 c8
    jbe short 02a0ah                          ; 76 0c
    mov ax, cx                                ; 89 c8
    sub word [bp+004h], cx                    ; 29 4e 04
    xor ax, cx                                ; 31 c8
    mov word [bp-014h], ax                    ; 89 46 ec
    jmp short 02a14h                          ; eb 0a
    mov cx, ax                                ; 89 c1
    mov word [bp+004h], strict word 00000h    ; c7 46 04 00 00
    sub word [bp-014h], ax                    ; 29 46 ec
    xor ax, ax                                ; 31 c0
    cmp word [bp+008h], strict byte 00000h    ; 83 7e 08 00
    jne short 02a3dh                          ; 75 21
    mov dx, word [bp-014h]                    ; 8b 56 ec
    cmp dx, word [bp+006h]                    ; 3b 56 06
    jbe short 02a3dh                          ; 76 19
    mov ax, word [bp-014h]                    ; 8b 46 ec
    sub ax, word [bp+006h]                    ; 2b 46 06
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov ax, word [bp+006h]                    ; 8b 46 06
    mov word [bp-014h], ax                    ; 89 46 ec
    xor ax, ax                                ; 31 c0
    mov word [bp+006h], ax                    ; 89 46 06
    mov word [bp+008h], ax                    ; 89 46 08
    jmp short 02a49h                          ; eb 0c
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov dx, word [bp-014h]                    ; 8b 56 ec
    sub word [bp+006h], dx                    ; 29 56 06
    sbb word [bp+008h], ax                    ; 19 46 08
    mov si, word [bp-014h]                    ; 8b 76 ec
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    test cl, 003h                             ; f6 c1 03
    je short 02a56h                           ; 74 02
    xor al, al                                ; 30 c0
    test byte [bp-014h], 003h                 ; f6 46 ec 03
    je short 02a5eh                           ; 74 02
    xor al, al                                ; 30 c0
    test byte [bp-00ch], 003h                 ; f6 46 f4 03
    je short 02a66h                           ; 74 02
    xor al, al                                ; 30 c0
    test byte [bp-014h], 001h                 ; f6 46 ec 01
    je short 02a7eh                           ; 74 12
    inc word [bp-014h]                        ; ff 46 ec
    cmp word [bp-00ch], strict byte 00000h    ; 83 7e f4 00
    jbe short 02a7eh                          ; 76 09
    test byte [bp-00ch], 001h                 ; f6 46 f4 01
    je short 02a7eh                           ; 74 03
    dec word [bp-00ch]                        ; ff 4e f4
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 02a8fh                          ; 75 0d
    shr word [bp-014h], 002h                  ; c1 6e ec 02
    shr cx, 002h                              ; c1 e9 02
    shr word [bp-00ch], 002h                  ; c1 6e f4 02
    jmp short 02a97h                          ; eb 08
    shr word [bp-014h], 1                     ; d1 6e ec
    shr cx, 1                                 ; d1 e9
    shr word [bp-00ch], 1                     ; d1 6e f4
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 02ac7h                          ; 75 2c
    test cx, cx                               ; 85 c9
    je short 02aa9h                           ; 74 0a
    mov dx, bx                                ; 89 da
    push eax                                  ; 66 50
    in eax, DX                                ; 66 ed
    loop 02aa3h                               ; e2 fc
    pop eax                                   ; 66 58
    mov dx, bx                                ; 89 da
    mov cx, word [bp-014h]                    ; 8b 4e ec
    les di, [bp+00ch]                         ; c4 7e 0c
    db  0f3h, 066h, 06dh
    ; rep insd                                  ; f3 66 6d
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    test ax, ax                               ; 85 c0
    je short 02ae6h                           ; 74 2b
    mov cx, ax                                ; 89 c1
    push eax                                  ; 66 50
    in eax, DX                                ; 66 ed
    loop 02abfh                               ; e2 fc
    pop eax                                   ; 66 58
    jmp short 02ae6h                          ; eb 1f
    test cx, cx                               ; 85 c9
    je short 02ad0h                           ; 74 05
    mov dx, bx                                ; 89 da
    in ax, DX                                 ; ed
    loop 02acdh                               ; e2 fd
    mov dx, bx                                ; 89 da
    mov cx, word [bp-014h]                    ; 8b 4e ec
    les di, [bp+00ch]                         ; c4 7e 0c
    rep insw                                  ; f3 6d
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    test ax, ax                               ; 85 c0
    je short 02ae6h                           ; 74 05
    mov cx, ax                                ; 89 c1
    in ax, DX                                 ; ed
    loop 02ae3h                               ; e2 fd
    add word [bp+00ch], si                    ; 01 76 0c
    xor ax, ax                                ; 31 c0
    add word [bp-018h], si                    ; 01 76 e8
    adc word [bp-016h], ax                    ; 11 46 ea
    mov ax, word [bp-018h]                    ; 8b 46 e8
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov si, word [bp-012h]                    ; 8b 76 ee
    mov word [es:si+016h], ax                 ; 26 89 44 16
    mov ax, word [bp-016h]                    ; 8b 46 ea
    mov word [es:si+018h], ax                 ; 26 89 44 18
    jmp near 02996h                           ; e9 8e fe
    mov al, dl                                ; 88 d0
    and AL, strict byte 0e9h                  ; 24 e9
    cmp AL, strict byte 040h                  ; 3c 40
    je short 02b1ch                           ; 74 0c
    mov dx, word [bp-010h]                    ; 8b 56 f0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    jmp near 02967h                           ; e9 4b fe
    mov dx, word [bp-010h]                    ; 8b 56 f0
    add dx, strict byte 00006h                ; 83 c2 06
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    xor dx, dx                                ; 31 d2
    mov ax, dx                                ; 89 d0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 0000ch                               ; c2 0c 00
set_diskette_ret_status_:                    ; 0xf2b32 LB 0x18
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00041h                ; ba 41 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 cb ea
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
set_diskette_current_cyl_:                   ; 0xf2b4a LB 0x2d
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    mov bl, al                                ; 88 c3
    cmp AL, strict byte 001h                  ; 3c 01
    jbe short 02b5fh                          ; 76 0b
    push 001b0h                               ; 68 b0 01
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 b1 ed
    add sp, strict byte 00004h                ; 83 c4 04
    movzx ax, dl                              ; 0f b6 c2
    movzx dx, bl                              ; 0f b6 d3
    add dx, 00094h                            ; 81 c2 94 00
    mov bx, ax                                ; 89 c3
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 9d ea
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
floppy_reset_controller_:                    ; 0xf2b77 LB 0x2b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov dx, 003f2h                            ; ba f2 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov bx, ax                                ; 89 c3
    movzx ax, bl                              ; 0f b6 c3
    and AL, strict byte 0fbh                  ; 24 fb
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    or AL, strict byte 004h                   ; 0c 04
    out DX, AL                                ; ee
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 080h                  ; 3c 80
    jne short 02b8fh                          ; 75 f4
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
floppy_prepare_controller_:                  ; 0xf2ba2 LB 0x8f
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push ax                                   ; 50
    mov cx, ax                                ; 89 c1
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 4c ea
    and AL, strict byte 07fh                  ; 24 7f
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 4c ea
    mov dx, 003f2h                            ; ba f2 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 004h                  ; 24 04
    mov byte [bp-008h], al                    ; 88 46 f8
    test cx, cx                               ; 85 c9
    je short 02bd5h                           ; 74 04
    mov AL, strict byte 020h                  ; b0 20
    jmp short 02bd7h                          ; eb 02
    mov AL, strict byte 010h                  ; b0 10
    or AL, strict byte 00ch                   ; 0c 0c
    or al, cl                                 ; 08 c8
    mov dx, 003f2h                            ; ba f2 03
    out DX, AL                                ; ee
    mov bx, strict word 00025h                ; bb 25 00
    mov dx, strict word 00040h                ; ba 40 00
    mov ax, dx                                ; 89 d0
    call 0160eh                               ; e8 24 ea
    mov dx, 0008bh                            ; ba 8b 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 0d ea
    shr al, 006h                              ; c0 e8 06
    mov dx, 003f7h                            ; ba f7 03
    out DX, AL                                ; ee
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 080h                  ; 3c 80
    jne short 02bfah                          ; 75 f4
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    jne short 02c29h                          ; 75 1d
    sti                                       ; fb
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 ea e9
    test AL, strict byte 080h                 ; a8 80
    je short 02c0dh                           ; 74 f3
    and AL, strict byte 07fh                  ; 24 7f
    cli                                       ; fa
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 e5 e9
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
floppy_media_known_:                         ; 0xf2c31 LB 0x43
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bx, ax                                ; 89 c3
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 bf e9
    mov ah, al                                ; 88 c4
    test bx, bx                               ; 85 db
    je short 02c49h                           ; 74 02
    shr al, 1                                 ; d0 e8
    and AL, strict byte 001h                  ; 24 01
    jne short 02c51h                          ; 75 04
    xor ah, ah                                ; 30 e4
    jmp short 02c6dh                          ; eb 1c
    mov dx, 00090h                            ; ba 90 00
    test bx, bx                               ; 85 db
    je short 02c5bh                           ; 74 03
    mov dx, 00091h                            ; ba 91 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 9f e9
    xor ah, ah                                ; 30 e4
    sar ax, 004h                              ; c1 f8 04
    and AL, strict byte 001h                  ; 24 01
    je short 02c4dh                           ; 74 e3
    mov ax, strict word 00001h                ; b8 01 00
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
floppy_read_id_:                             ; 0xf2c74 LB 0x4e
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    push si                                   ; 56
    sub sp, strict byte 00008h                ; 83 ec 08
    mov bx, ax                                ; 89 c3
    call 02ba2h                               ; e8 20 ff
    mov AL, strict byte 04ah                  ; b0 4a
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    out DX, AL                                ; ee
    sti                                       ; fb
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 6b e9
    and AL, strict byte 080h                  ; 24 80
    test al, al                               ; 84 c0
    je short 02c8ch                           ; 74 f1
    cli                                       ; fa
    xor si, si                                ; 31 f6
    jmp short 02ca5h                          ; eb 05
    cmp si, strict byte 00007h                ; 83 fe 07
    jnl short 02cb1h                          ; 7d 0c
    mov dx, 003f5h                            ; ba f5 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+si-00eh], al                 ; 88 42 f2
    inc si                                    ; 46
    jmp short 02ca0h                          ; eb ef
    test byte [bp-00eh], 0c0h                 ; f6 46 f2 c0
    db  00fh, 094h, 0c0h
    ; sete al                                   ; 0f 94 c0
    xor ah, ah                                ; 30 e4
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
floppy_drive_recal_:                         ; 0xf2cc2 LB 0x61
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    mov bx, ax                                ; 89 c3
    call 02ba2h                               ; e8 d5 fe
    mov AL, strict byte 007h                  ; b0 07
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    out DX, AL                                ; ee
    sti                                       ; fb
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 20 e9
    and AL, strict byte 080h                  ; 24 80
    test al, al                               ; 84 c0
    je short 02cd7h                           ; 74 f1
    cli                                       ; fa
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 10 e9
    and AL, strict byte 07fh                  ; 24 7f
    test bx, bx                               ; 85 db
    je short 02cfdh                           ; 74 07
    or AL, strict byte 002h                   ; 0c 02
    mov cx, 00095h                            ; b9 95 00
    jmp short 02d02h                          ; eb 05
    or AL, strict byte 001h                   ; 0c 01
    mov cx, 00094h                            ; b9 94 00
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 00 e9
    xor bx, bx                                ; 31 db
    mov dx, cx                                ; 89 ca
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 f6 e8
    mov ax, strict word 00001h                ; b8 01 00
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
floppy_media_sense_:                         ; 0xf2d23 LB 0xfd
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    mov di, ax                                ; 89 c7
    call 02cc2h                               ; e8 92 ff
    test ax, ax                               ; 85 c0
    jne short 02d39h                          ; 75 05
    xor cx, cx                                ; 31 c9
    jmp near 02e14h                           ; e9 db 00
    mov ax, strict word 00010h                ; b8 10 00
    call 0165ch                               ; e8 1d e9
    test di, di                               ; 85 ff
    jne short 02d4ah                          ; 75 07
    mov cl, al                                ; 88 c1
    shr cl, 004h                              ; c0 e9 04
    jmp short 02d4fh                          ; eb 05
    mov cl, al                                ; 88 c1
    and cl, 00fh                              ; 80 e1 0f
    cmp cl, 001h                              ; 80 f9 01
    jne short 02d5dh                          ; 75 09
    xor cl, cl                                ; 30 c9
    mov CH, strict byte 015h                  ; b5 15
    mov si, strict word 00001h                ; be 01 00
    jmp short 02da8h                          ; eb 4b
    cmp cl, 002h                              ; 80 f9 02
    jne short 02d68h                          ; 75 06
    xor cl, cl                                ; 30 c9
    mov CH, strict byte 035h                  ; b5 35
    jmp short 02d58h                          ; eb f0
    cmp cl, 003h                              ; 80 f9 03
    jne short 02d73h                          ; 75 06
    xor cl, cl                                ; 30 c9
    mov CH, strict byte 017h                  ; b5 17
    jmp short 02d58h                          ; eb e5
    cmp cl, 004h                              ; 80 f9 04
    jne short 02d7eh                          ; 75 06
    xor cl, cl                                ; 30 c9
    mov CH, strict byte 017h                  ; b5 17
    jmp short 02d58h                          ; eb da
    cmp cl, 005h                              ; 80 f9 05
    jne short 02d89h                          ; 75 06
    mov CL, strict byte 0cch                  ; b1 cc
    mov CH, strict byte 0d7h                  ; b5 d7
    jmp short 02d58h                          ; eb cf
    cmp cl, 006h                              ; 80 f9 06
    jne short 02d94h                          ; 75 06
    xor cl, cl                                ; 30 c9
    mov CH, strict byte 027h                  ; b5 27
    jmp short 02d58h                          ; eb c4
    cmp cl, 007h                              ; 80 f9 07
    jne short 02d9bh                          ; 75 02
    jmp short 02d8eh                          ; eb f3
    cmp cl, 008h                              ; 80 f9 08
    jne short 02da2h                          ; 75 02
    jmp short 02d8eh                          ; eb ec
    xor cl, cl                                ; 30 c9
    xor ch, ch                                ; 30 ed
    xor si, si                                ; 31 f6
    movzx bx, cl                              ; 0f b6 d9
    mov dx, 0008bh                            ; ba 8b 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 5a e8
    mov ax, di                                ; 89 f8
    call 02c74h                               ; e8 bb fe
    test ax, ax                               ; 85 c0
    jne short 02defh                          ; 75 32
    mov al, cl                                ; 88 c8
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 080h                  ; 3c 80
    je short 02defh                           ; 74 2a
    mov al, cl                                ; 88 c8
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 0c0h                  ; 3c c0
    je short 02ddch                           ; 74 0f
    mov ah, cl                                ; 88 cc
    and ah, 03fh                              ; 80 e4 3f
    cmp AL, strict byte 040h                  ; 3c 40
    je short 02de8h                           ; 74 12
    test al, al                               ; 84 c0
    je short 02de1h                           ; 74 07
    jmp short 02da8h                          ; eb cc
    and cl, 03fh                              ; 80 e1 3f
    jmp short 02da8h                          ; eb c7
    mov cl, ah                                ; 88 e1
    or cl, 040h                               ; 80 c9 40
    jmp short 02da8h                          ; eb c0
    mov cl, ah                                ; 88 e1
    or cl, 080h                               ; 80 c9 80
    jmp short 02da8h                          ; eb b9
    test di, di                               ; 85 ff
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    movzx di, al                              ; 0f b6 f8
    add di, 00090h                            ; 81 c7 90 00
    movzx bx, cl                              ; 0f b6 d9
    mov dx, 0008bh                            ; ba 8b 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 07 e8
    movzx bx, ch                              ; 0f b6 dd
    mov dx, di                                ; 89 fa
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 fc e7
    mov cx, si                                ; 89 f1
    mov ax, cx                                ; 89 c8
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
floppy_drive_exists_:                        ; 0xf2e20 LB 0x36
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    mov dx, ax                                ; 89 c2
    mov ax, strict word 00010h                ; b8 10 00
    call 0165ch                               ; e8 30 e8
    test dx, dx                               ; 85 d2
    jne short 02e35h                          ; 75 05
    shr al, 004h                              ; c0 e8 04
    jmp short 02e37h                          ; eb 02
    and AL, strict byte 00fh                  ; 24 0f
    test al, al                               ; 84 c0
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
    sysexit                                   ; 0f 35
    sbb dh, byte [di]                         ; 1a 35
    and AL, strict byte 035h                  ; 24 35
    sub si, word [di]                         ; 2b 35
    xor dh, byte [di]                         ; 32 35
    cmp word [di], si                         ; 39 35
    inc ax                                    ; 40
    xor ax, 0354ah                            ; 35 4a 35
    push cx                                   ; 51
    db  035h
_int13_diskette_function:                    ; 0xf2e56 LB 0x79e
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00012h                ; 83 ec 12
    mov bx, word [bp+016h]                    ; 8b 5e 16
    shr bx, 008h                              ; c1 eb 08
    mov ch, bl                                ; 88 dd
    mov si, word [bp+016h]                    ; 8b 76 16
    and si, 000ffh                            ; 81 e6 ff 00
    mov ah, byte [bp+00eh]                    ; 8a 66 0e
    cmp bl, 008h                              ; 80 fb 08
    jc short 02eafh                           ; 72 3a
    mov dx, word [bp+01ch]                    ; 8b 56 1c
    or dl, 001h                               ; 80 ca 01
    cmp bl, 008h                              ; 80 fb 08
    jbe near 03490h                           ; 0f 86 0e 06
    cmp bl, 016h                              ; 80 fb 16
    jc short 02ea5h                           ; 72 1e
    or si, 00100h                             ; 81 ce 00 01
    mov cx, si                                ; 89 f1
    cmp bl, 016h                              ; 80 fb 16
    jbe near 035afh                           ; 0f 86 1b 07
    cmp bl, 018h                              ; 80 fb 18
    je near 035b4h                            ; 0f 84 19 07
    cmp bl, 017h                              ; 80 fb 17
    je near 035b4h                            ; 0f 84 12 07
    jmp near 035d1h                           ; e9 2c 07
    cmp bl, 015h                              ; 80 fb 15
    je near 03573h                            ; 0f 84 c7 06
    jmp near 035d1h                           ; e9 22 07
    cmp bl, 001h                              ; 80 fb 01
    jc short 02ec9h                           ; 72 15
    jbe near 02f42h                           ; 0f 86 8a 00
    cmp bl, 005h                              ; 80 fb 05
    je near 032e9h                            ; 0f 84 2a 04
    cmp bl, 004h                              ; 80 fb 04
    jbe near 02f60h                           ; 0f 86 9a 00
    jmp near 035d1h                           ; e9 08 07
    test bl, bl                               ; 84 db
    jne near 035d1h                           ; 0f 85 02 07
    mov al, byte [bp+00eh]                    ; 8a 46 0e
    mov byte [bp-00eh], al                    ; 88 46 f2
    cmp AL, strict byte 001h                  ; 3c 01
    jbe short 02eedh                          ; 76 14
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, strict word 00001h                ; b8 01 00
    call 02b32h                               ; e8 48 fc
    jmp near 032bbh                           ; e9 ce 03
    mov ax, strict word 00010h                ; b8 10 00
    call 0165ch                               ; e8 69 e7
    cmp byte [bp-00eh], 000h                  ; 80 7e f2 00
    jne short 02f00h                          ; 75 07
    mov bl, al                                ; 88 c3
    shr bl, 004h                              ; c0 eb 04
    jmp short 02f05h                          ; eb 05
    mov bl, al                                ; 88 c3
    and bl, 00fh                              ; 80 e3 0f
    test bl, bl                               ; 84 db
    jne short 02f19h                          ; 75 10
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 080h                               ; 80 cc 80
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, 00080h                            ; b8 80 00
    jmp short 02ee7h                          ; eb ce
    xor bx, bx                                ; 31 db
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 ea e6
    xor al, al                                ; 30 c0
    mov byte [bp+017h], al                    ; 88 46 17
    xor ah, ah                                ; 30 e4
    call 02b32h                               ; e8 04 fc
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    xor dx, dx                                ; 31 d2
    call 02b4ah                               ; e8 0f fc
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    mov dx, 00441h                            ; ba 41 04
    xor ax, ax                                ; 31 c0
    call 01600h                               ; e8 b2 e6
    movzx dx, al                              ; 0f b6 d0
    sal dx, 008h                              ; c1 e2 08
    or si, dx                                 ; 09 d6
    mov word [bp+016h], si                    ; 89 76 16
    test al, al                               ; 84 c0
    je short 02f3bh                           ; 74 de
    jmp near 032bbh                           ; e9 5b 03
    mov al, byte [bp+016h]                    ; 8a 46 16
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov dx, word [bp+014h]                    ; 8b 56 14
    shr dx, 008h                              ; c1 ea 08
    mov byte [bp-008h], dl                    ; 88 56 f8
    mov al, byte [bp+014h]                    ; 8a 46 14
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov dx, word [bp+012h]                    ; 8b 56 12
    shr dx, 008h                              ; c1 ea 08
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov byte [bp-00eh], ah                    ; 88 66 f2
    cmp ah, 001h                              ; 80 fc 01
    jnbe short 02f96h                         ; 77 10
    cmp dl, 001h                              ; 80 fa 01
    jnbe short 02f96h                         ; 77 0b
    mov al, byte [bp-00ch]                    ; 8a 46 f4
    test al, al                               ; 84 c0
    je short 02f96h                           ; 74 04
    cmp AL, strict byte 048h                  ; 3c 48
    jbe short 02fbfh                          ; 76 29
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 2b e9
    push 001d5h                               ; 68 d5 01
    push 001edh                               ; 68 ed 01
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 61 e9
    add sp, strict byte 00006h                ; 83 c4 06
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, strict word 00001h                ; b8 01 00
    jmp short 03030h                          ; eb 71
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 02e20h                               ; e8 5a fe
    test ax, ax                               ; 85 c0
    je near 030c9h                            ; 0f 84 fd 00
    movzx dx, byte [bp-00eh]                  ; 0f b6 56 f2
    mov ax, dx                                ; 89 d0
    call 02c31h                               ; e8 5c fc
    test ax, ax                               ; 85 c0
    jne short 02ff2h                          ; 75 19
    mov ax, dx                                ; 89 d0
    call 02d23h                               ; e8 45 fd
    test ax, ax                               ; 85 c0
    jne short 02ff2h                          ; 75 10
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 00ch                               ; 80 cc 0c
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, strict word 0000ch                ; b8 0c 00
    jmp short 03030h                          ; eb 3e
    cmp ch, 002h                              ; 80 fd 02
    jne near 03187h                           ; 0f 85 8e 01
    mov dx, word [bp+006h]                    ; 8b 56 06
    shr dx, 00ch                              ; c1 ea 0c
    mov ah, dl                                ; 88 d4
    mov cx, word [bp+006h]                    ; 8b 4e 06
    sal cx, 004h                              ; c1 e1 04
    mov bx, word [bp+010h]                    ; 8b 5e 10
    add bx, cx                                ; 01 cb
    cmp bx, cx                                ; 39 cb
    jnc short 03012h                          ; 73 02
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    movzx cx, byte [bp-00ch]                  ; 0f b6 4e f4
    sal cx, 009h                              ; c1 e1 09
    dec cx                                    ; 49
    mov dx, bx                                ; 89 da
    add dx, cx                                ; 01 ca
    cmp dx, bx                                ; 39 da
    jnc short 0303ah                          ; 73 18
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 009h                               ; 80 cc 09
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, strict word 00009h                ; b8 09 00
    call 02b32h                               ; e8 ff fa
    mov byte [bp+016h], 000h                  ; c6 46 16 00
    jmp near 032bbh                           ; e9 81 02
    mov AL, strict byte 006h                  ; b0 06
    mov dx, strict word 0000ah                ; ba 0a 00
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    mov dx, strict word 0000ch                ; ba 0c 00
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    mov dx, strict word 00004h                ; ba 04 00
    out DX, AL                                ; ee
    shr bx, 008h                              ; c1 eb 08
    mov al, bl                                ; 88 d8
    out DX, AL                                ; ee
    xor al, bl                                ; 30 d8
    mov dx, strict word 0000ch                ; ba 0c 00
    out DX, AL                                ; ee
    mov al, cl                                ; 88 c8
    mov dx, strict word 00005h                ; ba 05 00
    out DX, AL                                ; ee
    shr cx, 008h                              ; c1 e9 08
    mov al, cl                                ; 88 c8
    out DX, AL                                ; ee
    mov AL, strict byte 046h                  ; b0 46
    mov dx, strict word 0000bh                ; ba 0b 00
    out DX, AL                                ; ee
    mov al, ah                                ; 88 e0
    mov dx, 00081h                            ; ba 81 00
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    mov dx, strict word 0000ah                ; ba 0a 00
    out DX, AL                                ; ee
    out DX, AL                                ; ee
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 02ba2h                               ; e8 24 fb
    mov AL, strict byte 0e6h                  ; b0 e6
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    sal dx, 002h                              ; c1 e2 02
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    or ax, dx                                 ; 09 d0
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    mov al, byte [bp-008h]                    ; 8a 46 f8
    out DX, AL                                ; ee
    mov al, byte [bp-006h]                    ; 8a 46 fa
    out DX, AL                                ; ee
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    out DX, AL                                ; ee
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    add ax, dx                                ; 01 d0
    dec ax                                    ; 48
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    mov AL, strict byte 0ffh                  ; b0 ff
    out DX, AL                                ; ee
    sti                                       ; fb
    mov dx, strict word 00040h                ; ba 40 00
    mov ax, dx                                ; 89 d0
    call 01600h                               ; e8 3e e5
    test al, al                               ; 84 c0
    jne short 030dah                          ; 75 14
    call 02b77h                               ; e8 ae fa
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 080h                               ; 80 cc 80
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, 00080h                            ; b8 80 00
    jmp near 03030h                           ; e9 56 ff
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 1d e5
    and AL, strict byte 080h                  ; 24 80
    test al, al                               ; 84 c0
    je short 030bah                           ; 74 d1
    cli                                       ; fa
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 0d e5
    and AL, strict byte 07fh                  ; 24 7f
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 0d e5
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 0c0h                  ; 3c c0
    je short 0311bh                           ; 74 0e
    push 001d5h                               ; 68 d5 01
    push 00208h                               ; 68 08 02
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 f5 e7
    add sp, strict byte 00006h                ; 83 c4 06
    xor si, si                                ; 31 f6
    jmp short 03124h                          ; eb 05
    cmp si, strict byte 00007h                ; 83 fe 07
    jnl short 0313ch                          ; 7d 18
    mov dx, 003f5h                            ; ba f5 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+si-016h], al                 ; 88 42 ea
    movzx bx, al                              ; 0f b6 d8
    lea dx, [si+042h]                         ; 8d 54 42
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 d5 e4
    inc si                                    ; 46
    jmp short 0311fh                          ; eb e3
    test byte [bp-016h], 0c0h                 ; f6 46 ea c0
    je short 03153h                           ; 74 11
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 020h                               ; 80 cc 20
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, strict word 00020h                ; b8 20 00
    jmp near 03030h                           ; e9 dd fe
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    sal ax, 009h                              ; c1 e0 09
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    mov cx, ax                                ; 89 c1
    mov si, word [bp+010h]                    ; 8b 76 10
    mov dx, word [bp+006h]                    ; 8b 56 06
    mov di, si                                ; 89 f7
    mov es, dx                                ; 8e c2
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsw                                 ; f3 a5
    pop DS                                    ; 1f
    movzx dx, byte [bp-008h]                  ; 0f b6 56 f8
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 02b4ah                               ; e8 ce f9
    mov byte [bp+017h], 000h                  ; c6 46 17 00
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    jmp near 02f3bh                           ; e9 b4 fd
    cmp ch, 003h                              ; 80 fd 03
    jne near 032d3h                           ; 0f 85 45 01
    mov dx, word [bp+006h]                    ; 8b 56 06
    shr dx, 00ch                              ; c1 ea 0c
    mov ah, dl                                ; 88 d4
    mov cx, word [bp+006h]                    ; 8b 4e 06
    sal cx, 004h                              ; c1 e1 04
    mov bx, word [bp+010h]                    ; 8b 5e 10
    add bx, cx                                ; 01 cb
    cmp bx, cx                                ; 39 cb
    jnc short 031a7h                          ; 73 02
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    movzx cx, byte [bp-00ch]                  ; 0f b6 4e f4
    sal cx, 009h                              ; c1 e1 09
    dec cx                                    ; 49
    mov dx, bx                                ; 89 da
    add dx, cx                                ; 01 ca
    cmp dx, bx                                ; 39 da
    jc near 03022h                            ; 0f 82 69 fe
    mov AL, strict byte 006h                  ; b0 06
    mov dx, strict word 0000ah                ; ba 0a 00
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    mov dx, strict word 0000ch                ; ba 0c 00
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    mov dx, strict word 00004h                ; ba 04 00
    out DX, AL                                ; ee
    shr bx, 008h                              ; c1 eb 08
    mov al, bl                                ; 88 d8
    out DX, AL                                ; ee
    xor al, bl                                ; 30 d8
    mov dx, strict word 0000ch                ; ba 0c 00
    out DX, AL                                ; ee
    mov al, cl                                ; 88 c8
    mov dx, strict word 00005h                ; ba 05 00
    out DX, AL                                ; ee
    shr cx, 008h                              ; c1 e9 08
    mov al, cl                                ; 88 c8
    out DX, AL                                ; ee
    mov AL, strict byte 04ah                  ; b0 4a
    mov dx, strict word 0000bh                ; ba 0b 00
    out DX, AL                                ; ee
    mov al, ah                                ; 88 e0
    mov dx, 00081h                            ; ba 81 00
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    mov dx, strict word 0000ah                ; ba 0a 00
    out DX, AL                                ; ee
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 02ba2h                               ; e8 a6 f9
    mov AL, strict byte 0c5h                  ; b0 c5
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    sal dx, 002h                              ; c1 e2 02
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    or ax, dx                                 ; 09 d0
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    mov al, byte [bp-008h]                    ; 8a 46 f8
    out DX, AL                                ; ee
    mov al, byte [bp-006h]                    ; 8a 46 fa
    out DX, AL                                ; ee
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    out DX, AL                                ; ee
    movzx dx, byte [bp-00ah]                  ; 0f b6 56 f6
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    add ax, dx                                ; 01 d0
    dec ax                                    ; 48
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    mov AL, strict byte 0ffh                  ; b0 ff
    out DX, AL                                ; ee
    sti                                       ; fb
    mov dx, strict word 00040h                ; ba 40 00
    mov ax, dx                                ; 89 d0
    call 01600h                               ; e8 c0 e3
    test al, al                               ; 84 c0
    je near 030c6h                            ; 0f 84 80 fe
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 b1 e3
    and AL, strict byte 080h                  ; 24 80
    test al, al                               ; 84 c0
    je short 03238h                           ; 74 e3
    cli                                       ; fa
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 a1 e3
    and AL, strict byte 07fh                  ; 24 7f
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 a1 e3
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 0c0h                  ; 3c c0
    je short 03287h                           ; 74 0e
    push 001d5h                               ; 68 d5 01
    push 00208h                               ; 68 08 02
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 89 e6
    add sp, strict byte 00006h                ; 83 c4 06
    xor si, si                                ; 31 f6
    jmp short 03290h                          ; eb 05
    cmp si, strict byte 00007h                ; 83 fe 07
    jnl short 032a8h                          ; 7d 18
    mov dx, 003f5h                            ; ba f5 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+si-016h], al                 ; 88 42 ea
    movzx bx, al                              ; 0f b6 d8
    lea dx, [si+042h]                         ; 8d 54 42
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 69 e3
    inc si                                    ; 46
    jmp short 0328bh                          ; eb e3
    test byte [bp-016h], 0c0h                 ; f6 46 ea c0
    je near 03171h                            ; 0f 84 c1 fe
    test byte [bp-015h], 002h                 ; f6 46 eb 02
    je short 032c2h                           ; 74 0c
    mov word [bp+016h], 00300h                ; c7 46 16 00 03
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    jmp near 02f3bh                           ; e9 79 fc
    push 001d5h                               ; 68 d5 01
    push 0021ch                               ; 68 1c 02
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 40 e6
    add sp, strict byte 00006h                ; 83 c4 06
    jmp near 03171h                           ; e9 9e fe
    movzx dx, byte [bp-008h]                  ; 0f b6 56 f8
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 02b4ah                               ; e8 6c f8
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    mov byte [bp+017h], 000h                  ; c6 46 17 00
    jmp near 02f3bh                           ; e9 52 fc
    mov al, byte [bp+016h]                    ; 8a 46 16
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov dx, word [bp+014h]                    ; 8b 56 14
    shr dx, 008h                              ; c1 ea 08
    mov ax, word [bp+012h]                    ; 8b 46 12
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp-006h], al                    ; 88 46 fa
    mov bl, byte [bp+00eh]                    ; 8a 5e 0e
    mov byte [bp-00eh], bl                    ; 88 5e f2
    cmp bl, 001h                              ; 80 fb 01
    jnbe short 0331dh                         ; 77 14
    cmp AL, strict byte 001h                  ; 3c 01
    jnbe short 0331dh                         ; 77 10
    cmp dl, 04fh                              ; 80 fa 4f
    jnbe short 0331dh                         ; 77 0b
    mov al, byte [bp-00ch]                    ; 8a 46 f4
    test al, al                               ; 84 c0
    je short 0331dh                           ; 74 04
    cmp AL, strict byte 012h                  ; 3c 12
    jbe short 03332h                          ; 76 15
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, strict word 00001h                ; b8 01 00
    call 02b32h                               ; e8 04 f8
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 02e20h                               ; e8 e7 fa
    test ax, ax                               ; 85 c0
    je near 02f09h                            ; 0f 84 ca fb
    movzx dx, byte [bp-00eh]                  ; 0f b6 56 f2
    mov ax, dx                                ; 89 d0
    call 02c31h                               ; e8 e9 f8
    test ax, ax                               ; 85 c0
    jne short 03357h                          ; 75 0b
    mov ax, dx                                ; 89 d0
    call 02d23h                               ; e8 d2 f9
    test ax, ax                               ; 85 c0
    je near 02fe2h                            ; 0f 84 8b fc
    mov dx, word [bp+006h]                    ; 8b 56 06
    shr dx, 00ch                              ; c1 ea 0c
    mov ah, dl                                ; 88 d4
    mov cx, word [bp+006h]                    ; 8b 4e 06
    sal cx, 004h                              ; c1 e1 04
    mov bx, word [bp+010h]                    ; 8b 5e 10
    add bx, cx                                ; 01 cb
    cmp bx, cx                                ; 39 cb
    jnc short 03370h                          ; 73 02
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    movzx cx, byte [bp-00ch]                  ; 0f b6 4e f4
    sal cx, 002h                              ; c1 e1 02
    dec cx                                    ; 49
    mov dx, bx                                ; 89 da
    add dx, cx                                ; 01 ca
    cmp dx, bx                                ; 39 da
    jc near 03022h                            ; 0f 82 a0 fc
    mov AL, strict byte 006h                  ; b0 06
    mov dx, strict word 0000ah                ; ba 0a 00
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    mov dx, strict word 0000ch                ; ba 0c 00
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    mov dx, strict word 00004h                ; ba 04 00
    out DX, AL                                ; ee
    shr bx, 008h                              ; c1 eb 08
    mov al, bl                                ; 88 d8
    out DX, AL                                ; ee
    xor al, bl                                ; 30 d8
    mov dx, strict word 0000ch                ; ba 0c 00
    out DX, AL                                ; ee
    mov al, cl                                ; 88 c8
    mov dx, strict word 00005h                ; ba 05 00
    out DX, AL                                ; ee
    shr cx, 008h                              ; c1 e9 08
    mov al, cl                                ; 88 c8
    out DX, AL                                ; ee
    mov AL, strict byte 04ah                  ; b0 4a
    mov dx, strict word 0000bh                ; ba 0b 00
    out DX, AL                                ; ee
    mov al, ah                                ; 88 e0
    mov dx, 00081h                            ; ba 81 00
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    mov dx, strict word 0000ah                ; ba 0a 00
    out DX, AL                                ; ee
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 02ba2h                               ; e8 dd f7
    mov AL, strict byte 04dh                  ; b0 4d
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    sal dx, 002h                              ; c1 e2 02
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    or ax, dx                                 ; 09 d0
    mov dx, 003f5h                            ; ba f5 03
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    out DX, AL                                ; ee
    mov al, byte [bp-00ch]                    ; 8a 46 f4
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    mov AL, strict byte 0f6h                  ; b0 f6
    out DX, AL                                ; ee
    sti                                       ; fb
    mov dx, strict word 00040h                ; ba 40 00
    mov ax, dx                                ; 89 d0
    call 01600h                               ; e8 0e e2
    test al, al                               ; 84 c0
    jne short 033fch                          ; 75 06
    call 02b77h                               ; e8 7e f7
    jmp near 02f09h                           ; e9 0d fb
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 fb e1
    and AL, strict byte 080h                  ; 24 80
    test al, al                               ; 84 c0
    je short 033eah                           ; 74 df
    cli                                       ; fa
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 eb e1
    and AL, strict byte 07fh                  ; 24 7f
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 0003eh                ; ba 3e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 eb e1
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 0c0h                  ; 3c c0
    je short 0343dh                           ; 74 0e
    push 001d5h                               ; 68 d5 01
    push 00208h                               ; 68 08 02
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 d3 e4
    add sp, strict byte 00006h                ; 83 c4 06
    xor si, si                                ; 31 f6
    jmp short 03446h                          ; eb 05
    cmp si, strict byte 00007h                ; 83 fe 07
    jnl short 0345eh                          ; 7d 18
    mov dx, 003f5h                            ; ba f5 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+si-016h], al                 ; 88 42 ea
    movzx bx, al                              ; 0f b6 d8
    lea dx, [si+042h]                         ; 8d 54 42
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 b3 e1
    inc si                                    ; 46
    jmp short 03441h                          ; eb e3
    test byte [bp-016h], 0c0h                 ; f6 46 ea c0
    je short 0347ah                           ; 74 16
    test byte [bp-015h], 002h                 ; f6 46 eb 02
    jne near 032b6h                           ; 0f 85 4a fe
    push 001d5h                               ; 68 d5 01
    push 0022ch                               ; 68 2c 02
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 96 e4
    add sp, strict byte 00006h                ; 83 c4 06
    xor al, al                                ; 30 c0
    mov byte [bp+017h], al                    ; 88 46 17
    xor ah, ah                                ; 30 e4
    call 02b32h                               ; e8 ae f6
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    xor dx, dx                                ; 31 d2
    call 02b4ah                               ; e8 bd f6
    jmp near 03180h                           ; e9 f0 fc
    mov byte [bp-00eh], ah                    ; 88 66 f2
    cmp ah, 001h                              ; 80 fc 01
    jbe short 034b8h                          ; 76 20
    xor ax, ax                                ; 31 c0
    mov word [bp+016h], ax                    ; 89 46 16
    mov word [bp+010h], ax                    ; 89 46 10
    mov word [bp+014h], ax                    ; 89 46 14
    mov word [bp+012h], ax                    ; 89 46 12
    mov word [bp+006h], ax                    ; 89 46 06
    mov word [bp+008h], ax                    ; 89 46 08
    movzx ax, cl                              ; 0f b6 c1
    mov word [bp+012h], ax                    ; 89 46 12
    mov word [bp+01ch], dx                    ; 89 56 1c
    jmp near 02f3bh                           ; e9 83 fa
    mov ax, strict word 00010h                ; b8 10 00
    call 0165ch                               ; e8 9e e1
    mov bl, al                                ; 88 c3
    xor cl, cl                                ; 30 c9
    test AL, strict byte 0f0h                 ; a8 f0
    je short 034c8h                           ; 74 02
    mov CL, strict byte 001h                  ; b1 01
    test bl, 00fh                             ; f6 c3 0f
    je short 034cfh                           ; 74 02
    db  0feh, 0c1h
    ; inc cl                                    ; fe c1
    cmp byte [bp-00eh], 000h                  ; 80 7e f2 00
    jne short 034dah                          ; 75 05
    shr bl, 004h                              ; c0 eb 04
    jmp short 034ddh                          ; eb 03
    and bl, 00fh                              ; 80 e3 0f
    xor al, al                                ; 30 c0
    mov byte [bp+011h], al                    ; 88 46 11
    movzx si, bl                              ; 0f b6 f3
    mov word [bp+010h], si                    ; 89 76 10
    xor ah, ah                                ; 30 e4
    mov word [bp+016h], ax                    ; 89 46 16
    mov dx, word [bp+012h]                    ; 8b 56 12
    xor dl, dl                                ; 30 d2
    movzx ax, cl                              ; 0f b6 c1
    or dx, ax                                 ; 09 c2
    mov word [bp+012h], dx                    ; 89 56 12
    cmp bl, 008h                              ; 80 fb 08
    jnbe short 03558h                         ; 77 59
    add si, si                                ; 01 f6
    mov ax, dx                                ; 89 d0
    xor ah, dh                                ; 30 f4
    mov bx, ax                                ; 89 c3
    or bh, 001h                               ; 80 cf 01
    jmp word [cs:si+02e44h]                   ; 2e ff a4 44 2e
    mov word [bp+014h], strict word 00000h    ; c7 46 14 00 00
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    jmp short 03566h                          ; eb 4c
    mov word [bp+014h], 02709h                ; c7 46 14 09 27
    mov word [bp+012h], bx                    ; 89 5e 12
    jmp short 03566h                          ; eb 42
    mov word [bp+014h], 04f0fh                ; c7 46 14 0f 4f
    jmp short 0351fh                          ; eb f4
    mov word [bp+014h], 04f09h                ; c7 46 14 09 4f
    jmp short 0351fh                          ; eb ed
    mov word [bp+014h], 04f12h                ; c7 46 14 12 4f
    jmp short 0351fh                          ; eb e6
    mov word [bp+014h], 04f24h                ; c7 46 14 24 4f
    jmp short 0351fh                          ; eb df
    mov word [bp+014h], 02708h                ; c7 46 14 08 27
    mov word [bp+012h], ax                    ; 89 46 12
    jmp short 03566h                          ; eb 1c
    mov word [bp+014h], 02709h                ; c7 46 14 09 27
    jmp short 03545h                          ; eb f4
    mov word [bp+014h], 02708h                ; c7 46 14 08 27
    jmp short 0351fh                          ; eb c7
    push 001d5h                               ; 68 d5 01
    push 0023dh                               ; 68 3d 02
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 aa e3
    add sp, strict byte 00006h                ; 83 c4 06
    mov word [bp+006h], 0f000h                ; c7 46 06 00 f0
    mov word [bp+008h], 0efc7h                ; c7 46 08 c7 ef
    jmp near 03180h                           ; e9 0d fc
    mov byte [bp-00eh], ah                    ; 88 66 f2
    cmp ah, 001h                              ; 80 fc 01
    jbe short 03581h                          ; 76 06
    mov word [bp+016h], si                    ; 89 76 16
    jmp near 034b2h                           ; e9 31 ff
    mov ax, strict word 00010h                ; b8 10 00
    call 0165ch                               ; e8 d5 e0
    cmp byte [bp-00eh], 000h                  ; 80 7e f2 00
    jne short 03594h                          ; 75 07
    mov bl, al                                ; 88 c3
    shr bl, 004h                              ; c0 eb 04
    jmp short 03599h                          ; eb 05
    mov bl, al                                ; 88 c3
    and bl, 00fh                              ; 80 e3 0f
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    test bl, bl                               ; 84 db
    je short 035a9h                           ; 74 03
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    jmp near 02f3bh                           ; e9 8c f9
    cmp ah, 001h                              ; 80 fc 01
    jbe short 035c0h                          ; 76 0c
    mov word [bp+016h], si                    ; 89 76 16
    mov ax, strict word 00001h                ; b8 01 00
    call 02b32h                               ; e8 75 f5
    jmp near 034b2h                           ; e9 f2 fe
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 006h                               ; 80 cc 06
    mov word [bp+016h], ax                    ; 89 46 16
    mov ax, strict word 00006h                ; b8 06 00
    jmp near 02ee7h                           ; e9 16 f9
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 f0 e2
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 001d5h                               ; 68 d5 01
    push 00252h                               ; 68 52 02
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 1f e3
    add sp, strict byte 00008h                ; 83 c4 08
    jmp near 02ed9h                           ; e9 e5 f8
_cdemu_init:                                 ; 0xf35f4 LB 0x18
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 1c e0
    xor bx, bx                                ; 31 db
    mov dx, 00322h                            ; ba 22 03
    call 0160eh                               ; e8 06 e0
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_cdemu_isactive:                             ; 0xf360c LB 0x16
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 04 e0
    mov dx, 00322h                            ; ba 22 03
    call 01600h                               ; e8 e2 df
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_cdemu_emulated_drive:                       ; 0xf3622 LB 0x16
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 ee df
    mov dx, 00324h                            ; ba 24 03
    call 01600h                               ; e8 cc df
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_int13_eltorito:                             ; 0xf3638 LB 0x189
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 d6 df
    mov si, 00322h                            ; be 22 03
    mov di, ax                                ; 89 c7
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    cmp ax, strict word 0004bh                ; 3d 4b 00
    jc short 03660h                           ; 72 0a
    jbe short 03686h                          ; 76 2e
    cmp ax, strict word 0004dh                ; 3d 4d 00
    jbe short 03667h                          ; 76 0a
    jmp near 03785h                           ; e9 25 01
    cmp ax, strict word 0004ah                ; 3d 4a 00
    jne near 03785h                           ; 0f 85 1e 01
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 5a e2
    push word [bp+016h]                       ; ff 76 16
    push 0026ch                               ; 68 6c 02
    push 0027bh                               ; 68 7b 02
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 8d e2
    add sp, strict byte 00008h                ; 83 c4 08
    jmp near 037a0h                           ; e9 1a 01
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    mov ax, word [bp+004h]                    ; 8b 46 04
    mov bx, strict word 00013h                ; bb 13 00
    call 0160eh                               ; e8 7c df
    mov es, di                                ; 8e c7
    movzx bx, byte [es:si+001h]               ; 26 0f b6 5c 01
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    inc dx                                    ; 42
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0160eh                               ; e8 6b df
    mov es, di                                ; 8e c7
    movzx bx, byte [es:si+002h]               ; 26 0f b6 5c 02
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    inc dx                                    ; 42
    inc dx                                    ; 42
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0160eh                               ; e8 59 df
    mov es, di                                ; 8e c7
    movzx bx, byte [es:si+003h]               ; 26 0f b6 5c 03
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 00003h                ; 83 c2 03
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0160eh                               ; e8 46 df
    mov es, di                                ; 8e c7
    mov bx, word [es:si+008h]                 ; 26 8b 5c 08
    mov cx, word [es:si+00ah]                 ; 26 8b 4c 0a
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 00004h                ; 83 c2 04
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0164ah                               ; e8 6c df
    mov es, di                                ; 8e c7
    mov bx, word [es:si+004h]                 ; 26 8b 5c 04
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 00008h                ; 83 c2 08
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0162ah                               ; e8 3a df
    mov es, di                                ; 8e c7
    mov bx, word [es:si+006h]                 ; 26 8b 5c 06
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 0000ah                ; 83 c2 0a
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0162ah                               ; e8 28 df
    mov es, di                                ; 8e c7
    mov bx, word [es:si+00ch]                 ; 26 8b 5c 0c
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 0000ch                ; 83 c2 0c
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0162ah                               ; e8 16 df
    mov es, di                                ; 8e c7
    mov bx, word [es:si+00eh]                 ; 26 8b 5c 0e
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 0000eh                ; 83 c2 0e
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0162ah                               ; e8 04 df
    mov es, di                                ; 8e c7
    movzx bx, byte [es:si+012h]               ; 26 0f b6 5c 12
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 00010h                ; 83 c2 10
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0160eh                               ; e8 d5 de
    mov es, di                                ; 8e c7
    movzx bx, byte [es:si+014h]               ; 26 0f b6 5c 14
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 00011h                ; 83 c2 11
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0160eh                               ; e8 c2 de
    mov es, di                                ; 8e c7
    movzx bx, byte [es:si+010h]               ; 26 0f b6 5c 10
    mov dx, word [bp+00ah]                    ; 8b 56 0a
    add dx, strict byte 00012h                ; 83 c2 12
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 0160eh                               ; e8 af de
    test byte [bp+016h], 0ffh                 ; f6 46 16 ff
    jne short 0376bh                          ; 75 06
    mov es, di                                ; 8e c7
    mov byte [es:si], 000h                    ; 26 c6 04 00
    mov byte [bp+017h], 000h                  ; c6 46 17 00
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 94 de
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 3c e1
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 0026ch                               ; 68 6c 02
    push 002a3h                               ; 68 a3 02
    jmp near 0367bh                           ; e9 db fe
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    mov bx, ax                                ; 89 c3
    shr bx, 008h                              ; c1 eb 08
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 53 de
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    jmp short 0377eh                          ; eb bd
device_is_cdrom_:                            ; 0xf37c1 LB 0x35
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bl, al                                ; 88 c3
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 4b de
    cmp bl, 010h                              ; 80 fb 10
    jc short 037dah                           ; 72 04
    xor ax, ax                                ; 31 c0
    jmp short 037efh                          ; eb 15
    xor bh, bh                                ; 30 ff
    imul bx, bx, strict byte 00018h           ; 6b db 18
    mov es, ax                                ; 8e c0
    add bx, 00122h                            ; 81 c3 22 01
    cmp byte [es:bx+01fh], 005h               ; 26 80 7f 1f 05
    jne short 037d6h                          ; 75 ea
    mov ax, strict word 00001h                ; b8 01 00
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
cdrom_boot_:                                 ; 0xf37f6 LB 0x435
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    sub sp, 0081ah                            ; 81 ec 1a 08
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 11 de
    mov word [bp-016h], ax                    ; 89 46 ea
    mov si, 00322h                            ; be 22 03
    mov word [bp-018h], ax                    ; 89 46 e8
    mov word [bp-012h], 00122h                ; c7 46 ee 22 01
    mov word [bp-010h], ax                    ; 89 46 f0
    mov byte [bp-00eh], 000h                  ; c6 46 f2 00
    jmp short 0382bh                          ; eb 09
    inc byte [bp-00eh]                        ; fe 46 f2
    cmp byte [bp-00eh], 010h                  ; 80 7e f2 10
    jnc short 03836h                          ; 73 0b
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    call 037c1h                               ; e8 8f ff
    test ax, ax                               ; 85 c0
    je short 03822h                           ; 74 ec
    cmp byte [bp-00eh], 010h                  ; 80 7e f2 10
    jc short 03842h                           ; 72 06
    mov ax, strict word 00002h                ; b8 02 00
    jmp near 03bc8h                           ; e9 86 03
    mov cx, strict word 0000ch                ; b9 0c 00
    xor bx, bx                                ; 31 db
    mov dx, ss                                ; 8c d2
    lea ax, [bp-024h]                         ; 8d 46 dc
    call 08e4ah                               ; e8 fb 55
    mov word [bp-024h], strict word 00028h    ; c7 46 dc 28 00
    mov ax, strict word 00011h                ; b8 11 00
    xor dx, dx                                ; 31 d2
    xchg ah, al                               ; 86 c4
    xchg dh, dl                               ; 86 d6
    xchg dx, ax                               ; 92
    mov word [bp-022h], ax                    ; 89 46 de
    mov word [bp-020h], dx                    ; 89 56 e0
    mov ax, strict word 00001h                ; b8 01 00
    xchg ah, al                               ; 86 c4
    mov word [bp-01dh], ax                    ; 89 46 e3
    les bx, [bp-012h]                         ; c4 5e ee
    db  066h, 026h, 0c7h, 047h, 00ah, 001h, 000h, 000h, 008h
    ; mov dword [es:bx+00ah], strict dword 008000001h ; 66 26 c7 47 0a 01 00 00 08
    mov byte [bp-00ch], 000h                  ; c6 46 f4 00
    jmp short 038a9h                          ; eb 2b
    lea dx, [bp-00824h]                       ; 8d 96 dc f7
    push SS                                   ; 16
    push dx                                   ; 52
    push strict byte 00001h                   ; 6a 01
    push strict byte 00000h                   ; 6a 00
    push 00800h                               ; 68 00 08
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov dx, strict word 0000ch                ; ba 0c 00
    call 0284ah                               ; e8 ae ef
    test ax, ax                               ; 85 c0
    je short 038cfh                           ; 74 2f
    inc byte [bp-00ch]                        ; fe 46 f4
    cmp byte [bp-00ch], 004h                  ; 80 7e f4 04
    jnbe short 038cfh                         ; 77 26
    cmp byte [bp-00eh], 008h                  ; 80 7e f2 08
    jbe short 0387eh                          ; 76 cf
    lea dx, [bp-00824h]                       ; 8d 96 dc f7
    push SS                                   ; 16
    push dx                                   ; 52
    push strict byte 00001h                   ; 6a 01
    push strict byte 00000h                   ; 6a 00
    push 00800h                               ; 68 00 08
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov dx, strict word 0000ch                ; ba 0c 00
    call 080a2h                               ; e8 d5 47
    jmp short 0389ch                          ; eb cd
    test ax, ax                               ; 85 c0
    je short 038d9h                           ; 74 06
    mov ax, strict word 00003h                ; b8 03 00
    jmp near 03bc8h                           ; e9 ef 02
    cmp byte [bp-00824h], 000h                ; 80 be dc f7 00
    je short 038e6h                           ; 74 06
    mov ax, strict word 00004h                ; b8 04 00
    jmp near 03bc8h                           ; e9 e2 02
    xor di, di                                ; 31 ff
    jmp short 038f0h                          ; eb 06
    inc di                                    ; 47
    cmp di, strict byte 00005h                ; 83 ff 05
    jnc short 03900h                          ; 73 10
    mov al, byte [bp+di-00823h]               ; 8a 83 dd f7
    cmp al, byte [di+00ce0h]                  ; 3a 85 e0 0c
    je short 038eah                           ; 74 f0
    mov ax, strict word 00005h                ; b8 05 00
    jmp near 03bc8h                           ; e9 c8 02
    xor di, di                                ; 31 ff
    jmp short 0390ah                          ; eb 06
    inc di                                    ; 47
    cmp di, strict byte 00017h                ; 83 ff 17
    jnc short 0391ah                          ; 73 10
    mov al, byte [bp+di-0081dh]               ; 8a 83 e3 f7
    cmp al, byte [di+00ce6h]                  ; 3a 85 e6 0c
    je short 03904h                           ; 74 f0
    mov ax, strict word 00006h                ; b8 06 00
    jmp near 03bc8h                           ; e9 ae 02
    mov ax, word [bp-007ddh]                  ; 8b 86 23 f8
    mov dx, word [bp-007dbh]                  ; 8b 96 25 f8
    mov word [bp-024h], strict word 00028h    ; c7 46 dc 28 00
    xchg ah, al                               ; 86 c4
    xchg dh, dl                               ; 86 d6
    xchg dx, ax                               ; 92
    mov word [bp-022h], ax                    ; 89 46 de
    mov word [bp-020h], dx                    ; 89 56 e0
    mov ax, strict word 00001h                ; b8 01 00
    xchg ah, al                               ; 86 c4
    mov word [bp-01dh], ax                    ; 89 46 e3
    cmp byte [bp-00eh], 008h                  ; 80 7e f2 08
    jbe short 03960h                          ; 76 20
    lea dx, [bp-00824h]                       ; 8d 96 dc f7
    push SS                                   ; 16
    push dx                                   ; 52
    push strict byte 00001h                   ; 6a 01
    push strict byte 00000h                   ; 6a 00
    push 00800h                               ; 68 00 08
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov dx, strict word 0000ch                ; ba 0c 00
    call 080a2h                               ; e8 44 47
    jmp short 0397eh                          ; eb 1e
    lea dx, [bp-00824h]                       ; 8d 96 dc f7
    push SS                                   ; 16
    push dx                                   ; 52
    push strict byte 00001h                   ; 6a 01
    push strict byte 00000h                   ; 6a 00
    push 00800h                               ; 68 00 08
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov dx, strict word 0000ch                ; ba 0c 00
    call 0284ah                               ; e8 cc ee
    test ax, ax                               ; 85 c0
    je short 03988h                           ; 74 06
    mov ax, strict word 00007h                ; b8 07 00
    jmp near 03bc8h                           ; e9 40 02
    cmp byte [bp-00824h], 001h                ; 80 be dc f7 01
    je short 03995h                           ; 74 06
    mov ax, strict word 00008h                ; b8 08 00
    jmp near 03bc8h                           ; e9 33 02
    cmp byte [bp-00823h], 000h                ; 80 be dd f7 00
    je short 039a2h                           ; 74 06
    mov ax, strict word 00009h                ; b8 09 00
    jmp near 03bc8h                           ; e9 26 02
    cmp byte [bp-00806h], 055h                ; 80 be fa f7 55
    je short 039afh                           ; 74 06
    mov ax, strict word 0000ah                ; b8 0a 00
    jmp near 03bc8h                           ; e9 19 02
    cmp byte [bp-00805h], 0aah                ; 80 be fb f7 aa
    jne short 039a9h                          ; 75 f3
    cmp byte [bp-00804h], 088h                ; 80 be fc f7 88
    je short 039c3h                           ; 74 06
    mov ax, strict word 0000bh                ; b8 0b 00
    jmp near 03bc8h                           ; e9 05 02
    mov al, byte [bp-00803h]                  ; 8a 86 fd f7
    mov es, [bp-018h]                         ; 8e 46 e8
    mov byte [es:si+001h], al                 ; 26 88 44 01
    cmp byte [bp-00803h], 000h                ; 80 be fd f7 00
    jne short 039dch                          ; 75 07
    mov byte [es:si+002h], 0e0h               ; 26 c6 44 02 e0
    jmp short 039efh                          ; eb 13
    cmp byte [bp-00803h], 004h                ; 80 be fd f7 04
    jnc short 039eah                          ; 73 07
    mov byte [es:si+002h], 000h               ; 26 c6 44 02 00
    jmp short 039efh                          ; eb 05
    mov byte [es:si+002h], 080h               ; 26 c6 44 02 80
    movzx di, byte [bp-00eh]                  ; 0f b6 7e f2
    mov ax, di                                ; 89 f8
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    mov es, [bp-018h]                         ; 8e 46 e8
    mov byte [es:si+003h], al                 ; 26 88 44 03
    mov ax, di                                ; 89 f8
    cwd                                       ; 99
    mov bx, strict word 00002h                ; bb 02 00
    idiv bx                                   ; f7 fb
    mov word [es:si+004h], dx                 ; 26 89 54 04
    mov di, word [bp-00802h]                  ; 8b be fe f7
    test di, di                               ; 85 ff
    jne short 03a18h                          ; 75 03
    mov di, 007c0h                            ; bf c0 07
    mov es, [bp-018h]                         ; 8e 46 e8
    mov word [es:si+00ch], di                 ; 26 89 7c 0c
    mov word [es:si+006h], strict word 00000h ; 26 c7 44 06 00 00
    mov ax, word [bp-007feh]                  ; 8b 86 02 f8
    mov word [bp-014h], ax                    ; 89 46 ec
    mov word [es:si+00eh], ax                 ; 26 89 44 0e
    mov ax, word [bp-007fch]                  ; 8b 86 04 f8
    mov dx, word [bp-007fah]                  ; 8b 96 06 f8
    mov word [es:si+008h], ax                 ; 26 89 44 08
    mov word [es:si+00ah], dx                 ; 26 89 54 0a
    mov word [bp-024h], strict word 00028h    ; c7 46 dc 28 00
    xchg ah, al                               ; 86 c4
    xchg dh, dl                               ; 86 d6
    xchg dx, ax                               ; 92
    mov word [bp-022h], ax                    ; 89 46 de
    mov word [bp-020h], dx                    ; 89 56 e0
    mov dx, word [bp-014h]                    ; 8b 56 ec
    dec dx                                    ; 4a
    shr dx, 002h                              ; c1 ea 02
    inc dx                                    ; 42
    mov ax, dx                                ; 89 d0
    xchg ah, al                               ; 86 c4
    mov word [bp-01dh], ax                    ; 89 46 e3
    les bx, [bp-012h]                         ; c4 5e ee
    mov word [es:bx+00ah], dx                 ; 26 89 57 0a
    mov word [es:bx+00ch], 00200h             ; 26 c7 47 0c 00 02
    mov ax, word [bp-014h]                    ; 8b 46 ec
    sal ax, 009h                              ; c1 e0 09
    and ah, 007h                              ; 80 e4 07
    mov dx, 00800h                            ; ba 00 08
    sub dx, ax                                ; 29 c2
    mov word [es:bx+01ch], dx                 ; 26 89 57 1c
    cmp byte [bp-00eh], 008h                  ; 80 7e f2 08
    jbe short 03aabh                          ; 76 27
    push di                                   ; 57
    push dword 000000001h                     ; 66 6a 01
    mov ax, word [bp-014h]                    ; 8b 46 ec
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00009h                ; b9 09 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 03a90h                               ; e2 fa
    push dx                                   ; 52
    push ax                                   ; 50
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov dx, strict word 0000ch                ; ba 0c 00
    call 080a2h                               ; e8 f9 45
    jmp short 03ad0h                          ; eb 25
    push di                                   ; 57
    push dword 000000001h                     ; 66 6a 01
    mov ax, word [bp-014h]                    ; 8b 46 ec
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00009h                ; b9 09 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 03ab7h                               ; e2 fa
    push dx                                   ; 52
    push ax                                   ; 50
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00eh]                  ; 0f b6 46 f2
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov dx, strict word 0000ch                ; ba 0c 00
    call 0284ah                               ; e8 7a ed
    les bx, [bp-012h]                         ; c4 5e ee
    mov word [es:bx+01ch], strict word 00000h ; 26 c7 47 1c 00 00
    test ax, ax                               ; 85 c0
    je short 03ae3h                           ; 74 06
    mov ax, strict word 0000ch                ; b8 0c 00
    jmp near 03bc8h                           ; e9 e5 00
    mov es, [bp-018h]                         ; 8e 46 e8
    mov al, byte [es:si+001h]                 ; 26 8a 44 01
    cmp AL, strict byte 002h                  ; 3c 02
    jc short 03afbh                           ; 72 0d
    jbe short 03b13h                          ; 76 23
    cmp AL, strict byte 004h                  ; 3c 04
    je short 03b29h                           ; 74 35
    cmp AL, strict byte 003h                  ; 3c 03
    je short 03b1eh                           ; 74 26
    jmp near 03b71h                           ; e9 76 00
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 03b71h                          ; 75 72
    mov es, [bp-018h]                         ; 8e 46 e8
    db  066h, 026h, 0c7h, 044h, 012h, 050h, 000h, 00fh, 000h
    ; mov dword [es:si+012h], strict dword 0000f0050h ; 66 26 c7 44 12 50 00 0f 00
    mov word [es:si+010h], strict word 00002h ; 26 c7 44 10 02 00
    jmp short 03b71h                          ; eb 5e
    db  066h, 026h, 0c7h, 044h, 012h, 050h, 000h, 012h, 000h
    ; mov dword [es:si+012h], strict dword 000120050h ; 66 26 c7 44 12 50 00 12 00
    jmp short 03b0bh                          ; eb ed
    db  066h, 026h, 0c7h, 044h, 012h, 050h, 000h, 024h, 000h
    ; mov dword [es:si+012h], strict dword 000240050h ; 66 26 c7 44 12 50 00 24 00
    jmp short 03b0bh                          ; eb e2
    mov dx, 001c4h                            ; ba c4 01
    mov ax, di                                ; 89 f8
    call 01600h                               ; e8 cf da
    and AL, strict byte 03fh                  ; 24 3f
    xor ah, ah                                ; 30 e4
    mov es, [bp-018h]                         ; 8e 46 e8
    mov word [es:si+014h], ax                 ; 26 89 44 14
    mov dx, 001c4h                            ; ba c4 01
    mov ax, di                                ; 89 f8
    call 01600h                               ; e8 bc da
    xor ah, ah                                ; 30 e4
    mov bx, ax                                ; 89 c3
    sal bx, 002h                              ; c1 e3 02
    mov dx, 001c5h                            ; ba c5 01
    mov ax, di                                ; 89 f8
    call 01600h                               ; e8 ad da
    xor ah, ah                                ; 30 e4
    add ax, bx                                ; 01 d8
    inc ax                                    ; 40
    mov es, [bp-018h]                         ; 8e 46 e8
    mov word [es:si+012h], ax                 ; 26 89 44 12
    mov dx, 001c3h                            ; ba c3 01
    mov ax, di                                ; 89 f8
    call 01600h                               ; e8 99 da
    xor ah, ah                                ; 30 e4
    inc ax                                    ; 40
    mov es, [bp-018h]                         ; 8e 46 e8
    mov word [es:si+010h], ax                 ; 26 89 44 10
    mov es, [bp-018h]                         ; 8e 46 e8
    cmp byte [es:si+001h], 000h               ; 26 80 7c 01 00
    je short 03bafh                           ; 74 34
    cmp byte [es:si+002h], 000h               ; 26 80 7c 02 00
    jne short 03b98h                          ; 75 16
    mov dx, strict word 00010h                ; ba 10 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 75 da
    or AL, strict byte 041h                   ; 0c 41
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00010h                ; ba 10 00
    mov ax, strict word 00040h                ; b8 40 00
    jmp short 03bach                          ; eb 14
    mov dx, 002c0h                            ; ba c0 02
    mov ax, word [bp-016h]                    ; 8b 46 ea
    call 01600h                               ; e8 5f da
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    movzx bx, al                              ; 0f b6 d8
    mov dx, 002c0h                            ; ba c0 02
    mov ax, word [bp-016h]                    ; 8b 46 ea
    call 0160eh                               ; e8 5f da
    mov es, [bp-018h]                         ; 8e 46 e8
    cmp byte [es:si+001h], 000h               ; 26 80 7c 01 00
    je short 03bbdh                           ; 74 04
    mov byte [es:si], 001h                    ; 26 c6 04 01
    mov es, [bp-018h]                         ; 8e 46 e8
    movzx ax, byte [es:si+002h]               ; 26 0f b6 44 02
    sal ax, 008h                              ; c1 e0 08
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
    push ax                                   ; 50
    dec si                                    ; 4e
    dec cx                                    ; 49
    dec ax                                    ; 48
    inc di                                    ; 47
    inc si                                    ; 46
    inc bp                                    ; 45
    inc sp                                    ; 44
    inc bx                                    ; 43
    inc dx                                    ; 42
    inc cx                                    ; 41
    sbb byte [01415h], dl                     ; 18 16 15 14
    adc word [bx+si], dx                      ; 11 10
    or ax, 00b0ch                             ; 0d 0c 0b
    or cl, byte [bx+di]                       ; 0a 09
    or byte [di], al                          ; 08 05
    add AL, strict byte 003h                  ; 04 03
    add al, byte [bx+di]                      ; 02 01
    add byte [bp-063c1h], bh                  ; 00 be 3f 9c
    aas                                       ; 3f
    fdivr qword [si]                          ; dc 3c
    push ES                                   ; 06
    cmp ax, 03cd1h                            ; 3d d1 3c
    push ES                                   ; 06
    cmp ax, 03cd1h                            ; 3d d1 3c
    clc                                       ; f8
    db  03eh, 09ch
    ; ds pushfw                                 ; 3e 9c
    aas                                       ; 3f
    mov si, 0be3fh                            ; be 3f be
    aas                                       ; 3f
    pushfw                                    ; 9c
    aas                                       ; 3f
    pushfw                                    ; 9c
    aas                                       ; 3f
    pushfw                                    ; 9c
    aas                                       ; 3f
    pushfw                                    ; 9c
    aas                                       ; 3f
    pushfw                                    ; 9c
    aas                                       ; 3f
    mov DH, strict byte 03fh                  ; b6 3f
    pushfw                                    ; 9c
    aas                                       ; 3f
    mov si, 0be3fh                            ; be 3f be
    aas                                       ; 3f
    mov si, 0be3fh                            ; be 3f be
    aas                                       ; 3f
    mov si, 0be3fh                            ; be 3f be
    aas                                       ; 3f
    mov si, 0be3fh                            ; be 3f be
    aas                                       ; 3f
    mov si, 0be3fh                            ; be 3f be
    aas                                       ; 3f
    mov si, 0be3fh                            ; be 3f be
    aas                                       ; 3f
_int13_cdemu:                                ; 0xf3c2b LB 0x42e
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0002ah                ; 83 ec 2a
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 e0 d9
    mov di, 00322h                            ; bf 22 03
    mov cx, ax                                ; 89 c1
    mov si, di                                ; 89 fe
    mov word [bp-008h], ax                    ; 89 46 f8
    mov word [bp-00ch], 00122h                ; c7 46 f4 22 01
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov es, ax                                ; 8e c0
    mov al, byte [es:di+003h]                 ; 26 8a 45 03
    add al, al                                ; 00 c0
    mov byte [bp-006h], al                    ; 88 46 fa
    mov al, byte [es:di+004h]                 ; 26 8a 45 04
    add byte [bp-006h], al                    ; 00 46 fa
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 a3 d9
    mov es, cx                                ; 8e c1
    cmp byte [es:di], 000h                    ; 26 80 3d 00
    je short 03c81h                           ; 74 0e
    movzx ax, byte [es:di+002h]               ; 26 0f b6 45 02
    mov dx, word [bp+012h]                    ; 8b 56 12
    xor dh, dh                                ; 30 f6
    cmp ax, dx                                ; 39 d0
    je short 03caah                           ; 74 29
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 40 dc
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 002bch                               ; 68 bc 02
    push 002c8h                               ; 68 c8 02
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 69 dc
    add sp, strict byte 0000ah                ; 83 c4 0a
    jmp near 03fdeh                           ; e9 34 03
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    cmp ax, strict word 00050h                ; 3d 50 00
    jnbe near 03fbeh                          ; 0f 87 07 03
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 0001eh                ; b9 1e 00
    mov di, 03bd2h                            ; bf d2 3b
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov ax, word [cs:di+03befh]               ; 2e 8b 85 ef 3b
    mov bx, word [bp+016h]                    ; 8b 5e 16
    xor bh, bh                                ; 30 ff
    jmp ax                                    ; ff e0
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 003h                               ; 80 cc 03
    jmp near 03fe6h                           ; e9 0a 03
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 1b d9
    mov cl, al                                ; 88 c1
    movzx ax, cl                              ; 0f b6 c1
    sal ax, 008h                              ; c1 e0 08
    or bx, ax                                 ; 09 c3
    mov word [bp+016h], bx                    ; 89 5e 16
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 11 d9
    test cl, cl                               ; 84 c9
    je near 03fa0h                            ; 0f 84 9d 02
    jmp near 03ffah                           ; e9 f4 02
    mov es, [bp-008h]                         ; 8e 46 f8
    mov di, word [es:si+014h]                 ; 26 8b 7c 14
    mov dx, word [es:si+012h]                 ; 26 8b 54 12
    mov bx, word [es:si+010h]                 ; 26 8b 5c 10
    mov ax, word [es:si+008h]                 ; 26 8b 44 08
    mov word [bp-018h], ax                    ; 89 46 e8
    mov ax, word [es:si+00ah]                 ; 26 8b 44 0a
    mov word [bp-016h], ax                    ; 89 46 ea
    mov ax, word [bp+014h]                    ; 8b 46 14
    and ax, strict word 0003fh                ; 25 3f 00
    mov word [bp-014h], ax                    ; 89 46 ec
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor ah, ah                                ; 30 e4
    and AL, strict byte 0c0h                  ; 24 c0
    sal ax, 002h                              ; c1 e0 02
    mov cx, word [bp+014h]                    ; 8b 4e 14
    shr cx, 008h                              ; c1 e9 08
    or ax, cx                                 ; 09 c8
    mov si, word [bp+012h]                    ; 8b 76 12
    shr si, 008h                              ; c1 ee 08
    mov cx, word [bp+016h]                    ; 8b 4e 16
    xor ch, ch                                ; 30 ed
    mov word [bp-00eh], cx                    ; 89 4e f2
    test cx, cx                               ; 85 c9
    je near 03f9ch                            ; 0f 84 4a 02
    cmp di, word [bp-014h]                    ; 3b 7e ec
    jc near 03fdeh                            ; 0f 82 85 02
    cmp ax, dx                                ; 39 d0
    jnc near 03fdeh                           ; 0f 83 7f 02
    cmp si, bx                                ; 39 de
    jnc near 03fdeh                           ; 0f 83 79 02
    mov dx, word [bp+016h]                    ; 8b 56 16
    shr dx, 008h                              ; c1 ea 08
    cmp dx, strict byte 00004h                ; 83 fa 04
    je near 03f9ch                            ; 0f 84 2a 02
    mov dx, word [bp+010h]                    ; 8b 56 10
    shr dx, 004h                              ; c1 ea 04
    mov cx, word [bp+006h]                    ; 8b 4e 06
    add cx, dx                                ; 01 d1
    mov word [bp-012h], cx                    ; 89 4e ee
    mov dx, word [bp+010h]                    ; 8b 56 10
    and dx, strict byte 0000fh                ; 83 e2 0f
    mov word [bp-010h], dx                    ; 89 56 f0
    xor dl, dl                                ; 30 d2
    xor cx, cx                                ; 31 c9
    call 08e19h                               ; e8 89 50
    xor bx, bx                                ; 31 db
    add ax, si                                ; 01 f0
    adc dx, bx                                ; 11 da
    mov bx, di                                ; 89 fb
    xor cx, cx                                ; 31 c9
    call 08e19h                               ; e8 7c 50
    mov bx, word [bp-014h]                    ; 8b 5e ec
    dec bx                                    ; 4b
    xor cx, cx                                ; 31 c9
    add ax, bx                                ; 01 d8
    adc dx, cx                                ; 11 ca
    mov bx, word [bp+016h]                    ; 8b 5e 16
    xor bl, bl                                ; 30 db
    mov cx, word [bp-00eh]                    ; 8b 4e f2
    or cx, bx                                 ; 09 d9
    mov word [bp+016h], cx                    ; 89 4e 16
    mov si, ax                                ; 89 c6
    mov di, dx                                ; 89 d7
    shr di, 1                                 ; d1 ef
    rcr si, 1                                 ; d1 de
    shr di, 1                                 ; d1 ef
    rcr si, 1                                 ; d1 de
    mov word [bp-01eh], di                    ; 89 7e e2
    mov di, ax                                ; 89 c7
    and di, strict byte 00003h                ; 83 e7 03
    xor bh, bh                                ; 30 ff
    add ax, word [bp-00eh]                    ; 03 46 f2
    adc dx, bx                                ; 11 da
    mov bx, ax                                ; 89 c3
    add bx, strict byte 0ffffh                ; 83 c3 ff
    mov ax, dx                                ; 89 d0
    adc ax, strict word 0ffffh                ; 15 ff ff
    mov word [bp-022h], bx                    ; 89 5e de
    mov word [bp-020h], ax                    ; 89 46 e0
    shr word [bp-020h], 1                     ; d1 6e e0
    rcr word [bp-022h], 1                     ; d1 5e de
    shr word [bp-020h], 1                     ; d1 6e e0
    rcr word [bp-022h], 1                     ; d1 5e de
    mov cx, strict word 0000ch                ; b9 0c 00
    xor bx, bx                                ; 31 db
    mov dx, ss                                ; 8c d2
    lea ax, [bp-02eh]                         ; 8d 46 d2
    call 08e4ah                               ; e8 52 50
    mov word [bp-02eh], strict word 00028h    ; c7 46 d2 28 00
    mov ax, word [bp-018h]                    ; 8b 46 e8
    add ax, si                                ; 01 f0
    mov dx, word [bp-016h]                    ; 8b 56 ea
    adc dx, word [bp-01eh]                    ; 13 56 e2
    xchg ah, al                               ; 86 c4
    xchg dh, dl                               ; 86 d6
    xchg dx, ax                               ; 92
    mov word [bp-02ch], ax                    ; 89 46 d4
    mov word [bp-02ah], dx                    ; 89 56 d6
    mov dx, word [bp-022h]                    ; 8b 56 de
    sub dx, si                                ; 29 f2
    inc dx                                    ; 42
    mov ax, dx                                ; 89 d0
    xchg ah, al                               ; 86 c4
    mov word [bp-027h], ax                    ; 89 46 d9
    les bx, [bp-00ch]                         ; c4 5e f4
    mov word [es:bx+00ah], dx                 ; 26 89 57 0a
    mov word [es:bx+00ch], 00200h             ; 26 c7 47 0c 00 02
    mov ax, di                                ; 89 f8
    sal ax, 009h                              ; c1 e0 09
    mov word [es:bx+01ah], ax                 ; 26 89 47 1a
    mov dx, word [bp-00eh]                    ; 8b 56 f2
    sal dx, 009h                              ; c1 e2 09
    and dh, 007h                              ; 80 e6 07
    mov bx, 00800h                            ; bb 00 08
    sub bx, dx                                ; 29 d3
    mov dx, bx                                ; 89 da
    mov bx, word [bp-00ch]                    ; 8b 5e f4
    mov bx, word [es:bx+01ah]                 ; 26 8b 5f 1a
    sub dx, bx                                ; 29 da
    mov bx, word [bp-00ch]                    ; 8b 5e f4
    mov word [es:bx+01ch], dx                 ; 26 89 57 1c
    cmp byte [bp-006h], 008h                  ; 80 7e fa 08
    jbe short 03e86h                          ; 76 2a
    push word [bp-012h]                       ; ff 76 ee
    push word [bp-010h]                       ; ff 76 f0
    push strict byte 00001h                   ; 6a 01
    mov si, word [bp-00eh]                    ; 8b 76 f2
    xor di, di                                ; 31 ff
    mov cx, strict word 00009h                ; b9 09 00
    sal si, 1                                 ; d1 e6
    rcl di, 1                                 ; d1 d7
    loop 03e6ch                               ; e2 fa
    push di                                   ; 57
    push si                                   ; 56
    push ax                                   ; 50
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    mov cx, ss                                ; 8c d1
    lea bx, [bp-02eh]                         ; 8d 5e d2
    mov dx, strict word 0000ch                ; ba 0c 00
    call 080a2h                               ; e8 1e 42
    jmp short 03eaeh                          ; eb 28
    push word [bp-012h]                       ; ff 76 ee
    push word [bp-010h]                       ; ff 76 f0
    push strict byte 00001h                   ; 6a 01
    mov si, word [bp-00eh]                    ; 8b 76 f2
    xor di, di                                ; 31 ff
    mov cx, strict word 00009h                ; b9 09 00
    sal si, 1                                 ; d1 e6
    rcl di, 1                                 ; d1 d7
    loop 03e96h                               ; e2 fa
    push di                                   ; 57
    push si                                   ; 56
    push ax                                   ; 50
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    mov cx, ss                                ; 8c d1
    lea bx, [bp-02eh]                         ; 8d 5e d2
    mov dx, strict word 0000ch                ; ba 0c 00
    call 0284ah                               ; e8 9c e9
    mov dl, al                                ; 88 c2
    les bx, [bp-00ch]                         ; c4 5e f4
    db  066h, 026h, 0c7h, 047h, 01ah, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+01ah], strict dword 000000000h ; 66 26 c7 47 1a 00 00 00 00
    test dl, dl                               ; 84 d2
    je near 03f9ch                            ; 0f 84 da 00
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 ff d9
    movzx ax, dl                              ; 0f b6 c2
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 002bch                               ; 68 bc 02
    push 002feh                               ; 68 fe 02
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 2a da
    add sp, strict byte 0000ah                ; 83 c4 0a
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 002h                               ; 80 cc 02
    mov word [bp+016h], ax                    ; 89 46 16
    mov byte [bp+016h], 000h                  ; c6 46 16 00
    jmp near 03fe9h                           ; e9 f1 00
    mov es, [bp-008h]                         ; 8e 46 f8
    mov di, word [es:si+014h]                 ; 26 8b 7c 14
    mov dx, word [es:si+012h]                 ; 26 8b 54 12
    dec dx                                    ; 4a
    mov bx, word [es:si+010h]                 ; 26 8b 5c 10
    dec bx                                    ; 4b
    mov byte [bp+016h], 000h                  ; c6 46 16 00
    mov ax, word [bp+010h]                    ; 8b 46 10
    xor al, al                                ; 30 c0
    mov cx, word [bp+014h]                    ; 8b 4e 14
    xor ch, ch                                ; 30 ed
    mov word [bp-01ch], cx                    ; 89 4e e4
    mov cx, dx                                ; 89 d1
    xor ch, dh                                ; 30 f5
    sal cx, 008h                              ; c1 e1 08
    mov word [bp-01ah], cx                    ; 89 4e e6
    mov cx, word [bp-01ch]                    ; 8b 4e e4
    or cx, word [bp-01ah]                     ; 0b 4e e6
    mov word [bp+014h], cx                    ; 89 4e 14
    shr dx, 002h                              ; c1 ea 02
    xor dh, dh                                ; 30 f6
    and dl, 0c0h                              ; 80 e2 c0
    mov word [bp-01ah], dx                    ; 89 56 e6
    mov dx, di                                ; 89 fa
    xor dh, dh                                ; 30 f6
    and dl, 03fh                              ; 80 e2 3f
    or dx, word [bp-01ah]                     ; 0b 56 e6
    xor cl, cl                                ; 30 c9
    or cx, dx                                 ; 09 d1
    mov word [bp+014h], cx                    ; 89 4e 14
    mov dx, word [bp+012h]                    ; 8b 56 12
    xor dh, dh                                ; 30 f6
    sal bx, 008h                              ; c1 e3 08
    or dx, bx                                 ; 09 da
    mov word [bp+012h], dx                    ; 89 56 12
    xor dl, dl                                ; 30 d2
    or dl, 002h                               ; 80 ca 02
    mov word [bp+012h], dx                    ; 89 56 12
    mov dl, byte [es:si+001h]                 ; 26 8a 54 01
    mov word [bp+010h], ax                    ; 89 46 10
    cmp dl, 003h                              ; 80 fa 03
    je short 03f84h                           ; 74 1a
    cmp dl, 002h                              ; 80 fa 02
    je short 03f80h                           ; 74 11
    cmp dl, 001h                              ; 80 fa 01
    jne short 03f88h                          ; 75 14
    mov ax, word [bp+010h]                    ; 8b 46 10
    xor al, al                                ; 30 c0
    or AL, strict byte 002h                   ; 0c 02
    mov word [bp+010h], ax                    ; 89 46 10
    jmp short 03f88h                          ; eb 08
    or AL, strict byte 004h                   ; 0c 04
    jmp short 03f7bh                          ; eb f7
    or AL, strict byte 005h                   ; 0c 05
    jmp short 03f7bh                          ; eb f3
    mov es, [bp-008h]                         ; 8e 46 f8
    cmp byte [es:si+001h], 004h               ; 26 80 7c 01 04
    jnc short 03f9ch                          ; 73 0a
    mov word [bp+008h], 0efc7h                ; c7 46 08 c7 ef
    mov word [bp+006h], 0f000h                ; c7 46 06 00 f0
    mov byte [bp+017h], 000h                  ; c6 46 17 00
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 63 d6
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    or bh, 003h                               ; 80 cf 03
    mov word [bp+016h], bx                    ; 89 5e 16
    jmp short 03fa0h                          ; eb e2
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 03 d9
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 002bch                               ; 68 bc 02
    push 0031fh                               ; 68 1f 03
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 32 d9
    add sp, strict byte 00008h                ; 83 c4 08
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    mov bx, word [bp+016h]                    ; 8b 5e 16
    shr bx, 008h                              ; c1 eb 08
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 14 d6
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    jmp short 03fafh                          ; eb af
    push ax                                   ; 50
    dec si                                    ; 4e
    dec cx                                    ; 49
    dec ax                                    ; 48
    inc di                                    ; 47
    inc si                                    ; 46
    inc bp                                    ; 45
    inc sp                                    ; 44
    inc bx                                    ; 43
    inc dx                                    ; 42
    inc cx                                    ; 41
    sbb byte [01415h], dl                     ; 18 16 15 14
    adc word [bx+si], dx                      ; 11 10
    or ax, 00b0ch                             ; 0d 0c 0b
    or cl, byte [bx+di]                       ; 0a 09
    or byte [di], al                          ; 08 05
    add AL, strict byte 003h                  ; 04 03
    add al, byte [bx+di]                      ; 02 01
    add byte [bp+02945h], ch                  ; 00 ae 45 29
    inc bx                                    ; 43
    push ES                                   ; 06
    inc cx                                    ; 41
    scasb                                     ; ae
    inc bp                                    ; 45
    sti                                       ; fb
    inc ax                                    ; 40
    scasb                                     ; ae
    inc bp                                    ; 45
    sti                                       ; fb
    inc ax                                    ; 40
    scasb                                     ; ae
    inc bp                                    ; 45
    sub word [bp+di-052h], ax                 ; 29 43 ae
    inc bp                                    ; 45
    scasb                                     ; ae
    inc bp                                    ; 45
    sub word [bp+di+029h], ax                 ; 29 43 29
    inc bx                                    ; 43
    sub word [bp+di+029h], ax                 ; 29 43 29
    inc bx                                    ; 43
    sub word [bp+di+030h], ax                 ; 29 43 30
    inc cx                                    ; 41
    sub word [bp+di-052h], ax                 ; 29 43 ae
    inc bp                                    ; 45
    cmp word [bx+di+04ch], ax                 ; 39 41 4c
    inc cx                                    ; 41
    sti                                       ; fb
    inc ax                                    ; 40
    dec sp                                    ; 4c
    inc cx                                    ; 41
    call far 04c43h:04342h                    ; 9a 42 43 43 4c
    inc cx                                    ; 41
    outsb                                     ; 6e
    inc bx                                    ; 43
    db  067h, 045h
    ; inc bp                                    ; 67 45
    outsw                                     ; 6f
    inc bp                                    ; 45
    scasb                                     ; ae
    inc bp                                    ; 45
_int13_cdrom:                                ; 0xf4059 LB 0x572
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00028h                ; 83 ec 28
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 b2 d5
    mov word [bp-01ch], ax                    ; 89 46 e4
    mov si, 00122h                            ; be 22 01
    mov word [bp-00eh], ax                    ; 89 46 f2
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 90 d5
    mov ax, word [bp+010h]                    ; 8b 46 10
    xor ah, ah                                ; 30 e4
    cmp ax, 000e0h                            ; 3d e0 00
    jc short 0408dh                           ; 72 05
    cmp ax, 000f0h                            ; 3d f0 00
    jc short 040abh                           ; 72 1e
    mov ax, word [bp+010h]                    ; 8b 46 10
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov ax, word [bp+018h]                    ; 8b 46 18
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 0034fh                               ; 68 4f 03
    push 0035bh                               ; 68 5b 03
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 68 d8
    add sp, strict byte 0000ah                ; 83 c4 0a
    jmp near 0458bh                           ; e9 e0 04
    mov ax, word [bp+010h]                    ; 8b 46 10
    xor ah, ah                                ; 30 e4
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov dl, byte [es:bx+000d0h]               ; 26 8a 97 d0 00
    mov byte [bp-00ah], dl                    ; 88 56 f6
    cmp dl, 010h                              ; 80 fa 10
    jc short 040d4h                           ; 72 10
    push ax                                   ; 50
    mov ax, word [bp+018h]                    ; 8b 46 18
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 0034fh                               ; 68 4f 03
    push 00386h                               ; 68 86 03
    jmp short 040a0h                          ; eb cc
    mov ax, word [bp+018h]                    ; 8b 46 18
    shr ax, 008h                              ; c1 e8 08
    cmp ax, strict word 00050h                ; 3d 50 00
    jnbe near 045aeh                          ; 0f 87 cd 04
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 0001eh                ; b9 1e 00
    mov di, 04000h                            ; bf 00 40
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov ax, word [cs:di+0401dh]               ; 2e 8b 85 1d 40
    mov bx, word [bp+018h]                    ; 8b 5e 18
    xor bh, bh                                ; 30 ff
    jmp ax                                    ; ff e0
    mov ax, word [bp+018h]                    ; 8b 46 18
    xor ah, ah                                ; 30 e4
    or ah, 003h                               ; 80 cc 03
    jmp near 04593h                           ; e9 8d 04
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 f1 d4
    mov cl, al                                ; 88 c1
    movzx ax, cl                              ; 0f b6 c1
    sal ax, 008h                              ; c1 e0 08
    or bx, ax                                 ; 09 c3
    mov word [bp+018h], bx                    ; 89 5e 18
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 e7 d4
    test cl, cl                               ; 84 c9
    je near 0432dh                            ; 0f 84 00 02
    jmp near 045a7h                           ; e9 77 04
    or bh, 002h                               ; 80 cf 02
    mov word [bp+018h], bx                    ; 89 5e 18
    jmp near 04596h                           ; e9 5d 04
    mov word [bp+012h], 0aa55h                ; c7 46 12 55 aa
    or bh, 030h                               ; 80 cf 30
    mov word [bp+018h], bx                    ; 89 5e 18
    mov word [bp+016h], strict word 00007h    ; c7 46 16 07 00
    jmp near 0432dh                           ; e9 e1 01
    mov bx, word [bp+00ch]                    ; 8b 5e 0c
    mov es, [bp+006h]                         ; 8e 46 06
    mov di, bx                                ; 89 df
    mov [bp-016h], es                         ; 8c 46 ea
    mov ax, word [es:bx+002h]                 ; 26 8b 47 02
    mov word [bp-010h], ax                    ; 89 46 f0
    mov ax, word [es:bx+006h]                 ; 26 8b 47 06
    mov word [bp-018h], ax                    ; 89 46 e8
    mov ax, word [es:bx+004h]                 ; 26 8b 47 04
    mov word [bp-01ah], ax                    ; 89 46 e6
    mov ax, word [es:bx+00ch]                 ; 26 8b 47 0c
    mov word [bp-014h], ax                    ; 89 46 ec
    mov ax, word [es:bx+00eh]                 ; 26 8b 47 0e
    mov word [bp-020h], ax                    ; 89 46 e0
    or ax, word [bp-014h]                     ; 0b 46 ec
    je short 04197h                           ; 74 18
    mov ax, word [bp+018h]                    ; 8b 46 18
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 0034fh                               ; 68 4f 03
    push 003b8h                               ; 68 b8 03
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 7c d7
    add sp, strict byte 00008h                ; 83 c4 08
    jmp near 0458bh                           ; e9 f4 03
    mov es, [bp-016h]                         ; 8e 46 ea
    mov ax, word [es:di+008h]                 ; 26 8b 45 08
    mov word [bp-014h], ax                    ; 89 46 ec
    mov ax, word [es:di+00ah]                 ; 26 8b 45 0a
    mov word [bp-020h], ax                    ; 89 46 e0
    mov ax, word [bp+018h]                    ; 8b 46 18
    shr ax, 008h                              ; c1 e8 08
    cmp ax, strict word 00044h                ; 3d 44 00
    je near 04329h                            ; 0f 84 74 01
    cmp ax, strict word 00047h                ; 3d 47 00
    je near 04329h                            ; 0f 84 6d 01
    mov cx, strict word 0000ch                ; b9 0c 00
    xor bx, bx                                ; 31 db
    mov dx, ss                                ; 8c d2
    lea ax, [bp-02ch]                         ; 8d 46 d4
    call 08e4ah                               ; e8 81 4c
    mov word [bp-02ch], strict word 00028h    ; c7 46 d4 28 00
    mov ax, word [bp-014h]                    ; 8b 46 ec
    mov dx, word [bp-020h]                    ; 8b 56 e0
    xchg ah, al                               ; 86 c4
    xchg dh, dl                               ; 86 d6
    xchg dx, ax                               ; 92
    mov word [bp-02ah], ax                    ; 89 46 d6
    mov word [bp-028h], dx                    ; 89 56 d8
    mov ax, word [bp-010h]                    ; 8b 46 f0
    xchg ah, al                               ; 86 c4
    mov word [bp-025h], ax                    ; 89 46 db
    les ax, [bp-010h]                         ; c4 46 f0
    mov word [es:si+00ah], ax                 ; 26 89 44 0a
    mov word [es:si+00ch], 00800h             ; 26 c7 44 0c 00 08
    cmp byte [bp-00ah], 008h                  ; 80 7e f6 08
    jbe short 04220h                          ; 76 26
    push dword [bp-01ah]                      ; 66 ff 76 e6
    push strict byte 00001h                   ; 6a 01
    xor bx, bx                                ; 31 db
    mov cx, strict word 0000bh                ; b9 0b 00
    sal ax, 1                                 ; d1 e0
    rcl bx, 1                                 ; d1 d3
    loop 04205h                               ; e2 fa
    push bx                                   ; 53
    push ax                                   ; 50
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    mov cx, ss                                ; 8c d1
    lea bx, [bp-02ch]                         ; 8d 5e d4
    mov dx, strict word 0000ch                ; ba 0c 00
    call 080a2h                               ; e8 84 3e
    jmp short 04244h                          ; eb 24
    push dword [bp-01ah]                      ; 66 ff 76 e6
    push strict byte 00001h                   ; 6a 01
    xor bx, bx                                ; 31 db
    mov cx, strict word 0000bh                ; b9 0b 00
    sal ax, 1                                 ; d1 e0
    rcl bx, 1                                 ; d1 d3
    loop 0422bh                               ; e2 fa
    push bx                                   ; 53
    push ax                                   ; 50
    push strict byte 00000h                   ; 6a 00
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    mov cx, ss                                ; 8c d1
    lea bx, [bp-02ch]                         ; 8d 5e d4
    mov dx, strict word 0000ch                ; ba 0c 00
    call 0284ah                               ; e8 06 e6
    mov byte [bp-008h], al                    ; 88 46 f8
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov ax, word [es:si+016h]                 ; 26 8b 44 16
    mov bx, word [es:si+018h]                 ; 26 8b 5c 18
    mov cx, strict word 0000bh                ; b9 0b 00
    shr bx, 1                                 ; d1 eb
    rcr ax, 1                                 ; d1 d8
    loop 04255h                               ; e2 fa
    mov es, [bp-016h]                         ; 8e 46 ea
    mov word [es:di+002h], ax                 ; 26 89 45 02
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    je near 04329h                            ; 0f 84 bf 00
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 57 d6
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    mov ax, word [bp+018h]                    ; 8b 46 18
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 0034fh                               ; 68 4f 03
    push 003e1h                               ; 68 e1 03
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 81 d6
    add sp, strict byte 0000ah                ; 83 c4 0a
    mov ax, word [bp+018h]                    ; 8b 46 18
    xor ah, ah                                ; 30 e4
    or ah, 00ch                               ; 80 cc 0c
    jmp near 04593h                           ; e9 f9 02
    cmp bx, strict byte 00002h                ; 83 fb 02
    jnbe near 0458bh                          ; 0f 87 ea 02
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov di, si                                ; 89 f7
    add di, ax                                ; 01 c7
    mov al, byte [es:di+021h]                 ; 26 8a 45 21
    cmp bx, strict byte 00002h                ; 83 fb 02
    je short 0431ah                           ; 74 62
    cmp bx, strict byte 00001h                ; 83 fb 01
    je short 042f8h                           ; 74 3b
    test bx, bx                               ; 85 db
    jne near 04329h                           ; 0f 85 66 00
    cmp AL, strict byte 0ffh                  ; 3c ff
    jne short 042d9h                          ; 75 12
    mov ax, word [bp+018h]                    ; 8b 46 18
    xor ah, ah                                ; 30 e4
    or ah, 0b4h                               ; 80 cc b4
    mov word [bp+018h], ax                    ; 89 46 18
    xor al, al                                ; 30 c0
    or AL, strict byte 001h                   ; 0c 01
    jmp near 04593h                           ; e9 ba 02
    movzx dx, byte [bp-00ah]                  ; 0f b6 56 f6
    imul dx, dx, strict byte 00018h           ; 6b d2 18
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    mov es, [bp-00eh]                         ; 8e 46 f2
    add si, dx                                ; 01 d6
    mov byte [es:si+021h], al                 ; 26 88 44 21
    mov ax, word [bp+018h]                    ; 8b 46 18
    xor al, al                                ; 30 c0
    or AL, strict byte 001h                   ; 0c 01
    mov word [bp+018h], ax                    ; 89 46 18
    jmp near 04329h                           ; e9 31 00
    test al, al                               ; 84 c0
    jne short 04308h                          ; 75 0c
    or bh, 0b0h                               ; 80 cf b0
    mov word [bp+018h], bx                    ; 89 5e 18
    mov byte [bp+018h], al                    ; 88 46 18
    jmp near 04596h                           ; e9 8e 02
    movzx dx, byte [bp-00ah]                  ; 0f b6 56 f6
    imul dx, dx, strict byte 00018h           ; 6b d2 18
    db  0feh, 0c8h
    ; dec al                                    ; fe c8
    mov es, [bp-00eh]                         ; 8e 46 f2
    add si, dx                                ; 01 d6
    mov byte [es:si+021h], al                 ; 26 88 44 21
    test al, al                               ; 84 c0
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    mov dx, word [bp+018h]                    ; 8b 56 18
    mov dl, al                                ; 88 c2
    mov word [bp+018h], dx                    ; 89 56 18
    mov byte [bp+019h], 000h                  ; c6 46 19 00
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 d6 d2
    and byte [bp+01eh], 0feh                  ; 80 66 1e fe
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-00eh]                         ; 8e 46 f2
    add si, ax                                ; 01 c6
    mov al, byte [es:si+021h]                 ; 26 8a 44 21
    test al, al                               ; 84 c0
    je short 0435dh                           ; 74 06
    or bh, 0b1h                               ; 80 cf b1
    jmp near 04133h                           ; e9 d6 fd
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    je short 04329h                           ; 74 c6
    mov ax, word [bp+018h]                    ; 8b 46 18
    xor ah, ah                                ; 30 e4
    or ah, 0b1h                               ; 80 cc b1
    jmp near 04593h                           ; e9 25 02
    mov dx, word [bp+00ch]                    ; 8b 56 0c
    mov cx, word [bp+006h]                    ; 8b 4e 06
    mov bx, dx                                ; 89 d3
    mov word [bp-00ch], cx                    ; 89 4e f4
    mov es, cx                                ; 8e c1
    mov di, dx                                ; 89 d7
    mov ax, word [es:di]                      ; 26 8b 05
    mov word [bp-012h], ax                    ; 89 46 ee
    cmp ax, strict word 0001ah                ; 3d 1a 00
    jc near 0458bh                            ; 0f 82 01 02
    jc short 043d9h                           ; 72 4d
    movzx di, byte [bp-00ah]                  ; 0f b6 7e f6
    imul di, di, strict byte 00018h           ; 6b ff 18
    mov es, [bp-00eh]                         ; 8e 46 f2
    add di, si                                ; 01 f7
    mov ax, word [es:di+024h]                 ; 26 8b 45 24
    mov es, cx                                ; 8e c1
    mov di, dx                                ; 89 d7
    db  066h, 026h, 0c7h, 005h, 01ah, 000h, 074h, 000h
    ; mov dword [es:di], strict dword 00074001ah ; 66 26 c7 05 1a 00 74 00
    db  066h, 026h, 0c7h, 045h, 004h, 0ffh, 0ffh, 0ffh, 0ffh
    ; mov dword [es:di+004h], strict dword 0ffffffffh ; 66 26 c7 45 04 ff ff ff ff
    db  066h, 026h, 0c7h, 045h, 008h, 0ffh, 0ffh, 0ffh, 0ffh
    ; mov dword [es:di+008h], strict dword 0ffffffffh ; 66 26 c7 45 08 ff ff ff ff
    db  066h, 026h, 0c7h, 045h, 00ch, 0ffh, 0ffh, 0ffh, 0ffh
    ; mov dword [es:di+00ch], strict dword 0ffffffffh ; 66 26 c7 45 0c ff ff ff ff
    mov word [es:di+018h], ax                 ; 26 89 45 18
    db  066h, 026h, 0c7h, 045h, 010h, 0ffh, 0ffh, 0ffh, 0ffh
    ; mov dword [es:di+010h], strict dword 0ffffffffh ; 66 26 c7 45 10 ff ff ff ff
    db  066h, 026h, 0c7h, 045h, 014h, 0ffh, 0ffh, 0ffh, 0ffh
    ; mov dword [es:di+014h], strict dword 0ffffffffh ; 66 26 c7 45 14 ff ff ff ff
    cmp word [bp-012h], strict byte 0001eh    ; 83 7e ee 1e
    jc near 044afh                            ; 0f 82 ce 00
    mov es, [bp-00ch]                         ; 8e 46 f4
    mov word [es:bx], strict word 0001eh      ; 26 c7 07 1e 00
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    mov word [es:bx+01ch], ax                 ; 26 89 47 1c
    mov word [es:bx+01ah], 00312h             ; 26 c7 47 1a 12 03
    movzx cx, byte [bp-00ah]                  ; 0f b6 4e f6
    mov ax, cx                                ; 89 c8
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    xor ah, ah                                ; 30 e4
    imul ax, ax, strict byte 00006h           ; 6b c0 06
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov di, si                                ; 89 f7
    add di, ax                                ; 01 c7
    mov dx, word [es:di+001c2h]               ; 26 8b 95 c2 01
    mov ax, word [es:di+001c4h]               ; 26 8b 85 c4 01
    mov word [bp-01eh], ax                    ; 89 46 e2
    mov al, byte [es:di+001c1h]               ; 26 8a 85 c1 01
    mov byte [bp-006h], al                    ; 88 46 fa
    imul cx, cx, strict byte 00018h           ; 6b c9 18
    mov di, si                                ; 89 f7
    add di, cx                                ; 01 cf
    mov al, byte [es:di+022h]                 ; 26 8a 45 22
    cmp AL, strict byte 001h                  ; 3c 01
    db  00fh, 094h, 0c0h
    ; sete al                                   ; 0f 94 c0
    xor ah, ah                                ; 30 e4
    or AL, strict byte 070h                   ; 0c 70
    mov di, ax                                ; 89 c7
    mov word [es:si+001f0h], dx               ; 26 89 94 f0 01
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    mov word [es:si+001f2h], ax               ; 26 89 84 f2 01
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    cwd                                       ; 99
    mov cx, strict word 00002h                ; b9 02 00
    idiv cx                                   ; f7 f9
    or dl, 00eh                               ; 80 ca 0e
    sal dx, 004h                              ; c1 e2 04
    mov byte [es:si+001f4h], dl               ; 26 88 94 f4 01
    mov byte [es:si+001f5h], 0cbh             ; 26 c6 84 f5 01 cb
    mov al, byte [bp-006h]                    ; 8a 46 fa
    mov byte [es:si+001f6h], al               ; 26 88 84 f6 01
    mov word [es:si+001f7h], strict word 00001h ; 26 c7 84 f7 01 01 00
    mov byte [es:si+001f9h], 000h             ; 26 c6 84 f9 01 00
    mov word [es:si+001fah], di               ; 26 89 bc fa 01
    mov word [es:si+001fch], strict word 00000h ; 26 c7 84 fc 01 00 00
    mov byte [es:si+001feh], 011h             ; 26 c6 84 fe 01 11
    xor cl, cl                                ; 30 c9
    xor ch, ch                                ; 30 ed
    jmp short 04492h                          ; eb 05
    cmp ch, 00fh                              ; 80 fd 0f
    jnc short 044a5h                          ; 73 13
    movzx dx, ch                              ; 0f b6 d5
    add dx, 00312h                            ; 81 c2 12 03
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    call 01600h                               ; e8 61 d1
    add cl, al                                ; 00 c1
    db  0feh, 0c5h
    ; inc ch                                    ; fe c5
    jmp short 0448dh                          ; eb e8
    neg cl                                    ; f6 d9
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov byte [es:si+001ffh], cl               ; 26 88 8c ff 01
    cmp word [bp-012h], strict byte 00042h    ; 83 7e ee 42
    jc near 04329h                            ; 0f 82 72 fe
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    xor ah, ah                                ; 30 e4
    imul ax, ax, strict byte 00006h           ; 6b c0 06
    mov es, [bp-00eh]                         ; 8e 46 f2
    add si, ax                                ; 01 c6
    mov al, byte [es:si+001c0h]               ; 26 8a 84 c0 01
    mov dx, word [es:si+001c2h]               ; 26 8b 94 c2 01
    mov es, [bp-00ch]                         ; 8e 46 f4
    mov word [es:bx], strict word 00042h      ; 26 c7 07 42 00
    db  066h, 026h, 0c7h, 047h, 01eh, 0ddh, 0beh, 024h, 000h
    ; mov dword [es:bx+01eh], strict dword 00024beddh ; 66 26 c7 47 1e dd be 24 00
    mov word [es:bx+022h], strict word 00000h ; 26 c7 47 22 00 00
    test al, al                               ; 84 c0
    jne short 044f8h                          ; 75 09
    db  066h, 026h, 0c7h, 047h, 024h, 049h, 053h, 041h, 020h
    ; mov dword [es:bx+024h], strict dword 020415349h ; 66 26 c7 47 24 49 53 41 20
    mov es, [bp-00ch]                         ; 8e 46 f4
    db  066h, 026h, 0c7h, 047h, 028h, 041h, 054h, 041h, 020h
    ; mov dword [es:bx+028h], strict dword 020415441h ; 66 26 c7 47 28 41 54 41 20
    db  066h, 026h, 0c7h, 047h, 02ch, 020h, 020h, 020h, 020h
    ; mov dword [es:bx+02ch], strict dword 020202020h ; 66 26 c7 47 2c 20 20 20 20
    test al, al                               ; 84 c0
    jne short 04524h                          ; 75 13
    mov word [es:bx+030h], dx                 ; 26 89 57 30
    db  066h, 026h, 0c7h, 047h, 032h, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+032h], strict dword 000000000h ; 66 26 c7 47 32 00 00 00 00
    mov word [es:bx+036h], strict word 00000h ; 26 c7 47 36 00 00
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    and AL, strict byte 001h                  ; 24 01
    xor ah, ah                                ; 30 e4
    mov es, [bp-00ch]                         ; 8e 46 f4
    mov word [es:bx+038h], ax                 ; 26 89 47 38
    db  066h, 026h, 0c7h, 047h, 03ah, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+03ah], strict dword 000000000h ; 66 26 c7 47 3a 00 00 00 00
    mov word [es:bx+03eh], strict word 00000h ; 26 c7 47 3e 00 00
    xor al, al                                ; 30 c0
    mov AH, strict byte 01eh                  ; b4 1e
    jmp short 0454ch                          ; eb 05
    cmp ah, 040h                              ; 80 fc 40
    jnc short 0455bh                          ; 73 0f
    movzx si, ah                              ; 0f b6 f4
    mov es, [bp-00ch]                         ; 8e 46 f4
    add si, bx                                ; 01 de
    add al, byte [es:si]                      ; 26 02 04
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    jmp short 04547h                          ; eb ec
    neg al                                    ; f6 d8
    mov es, [bp-00ch]                         ; 8e 46 f4
    mov byte [es:bx+041h], al                 ; 26 88 47 41
    jmp near 04329h                           ; e9 c2 fd
    or bh, 006h                               ; 80 cf 06
    mov word [bp+018h], bx                    ; 89 5e 18
    jmp short 045a7h                          ; eb 38
    cmp bx, strict byte 00006h                ; 83 fb 06
    je near 04329h                            ; 0f 84 b3 fd
    cmp bx, strict byte 00001h                ; 83 fb 01
    jc short 0458bh                           ; 72 10
    jbe near 04329h                           ; 0f 86 aa fd
    cmp bx, strict byte 00003h                ; 83 fb 03
    jc short 0458bh                           ; 72 07
    cmp bx, strict byte 00004h                ; 83 fb 04
    jbe near 04329h                           ; 0f 86 9e fd
    mov ax, word [bp+018h]                    ; 8b 46 18
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+018h], ax                    ; 89 46 18
    mov bx, word [bp+018h]                    ; 8b 5e 18
    shr bx, 008h                              ; c1 eb 08
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 67 d0
    or byte [bp+01eh], 001h                   ; 80 4e 1e 01
    jmp near 0433ch                           ; e9 8e fd
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 13 d3
    mov ax, word [bp+018h]                    ; 8b 46 18
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 0034fh                               ; 68 4f 03
    push 002a3h                               ; 68 a3 02
    push strict byte 00004h                   ; 6a 04
    jmp near 0418eh                           ; e9 c3 fb
print_boot_device_:                          ; 0xf45cb LB 0x4b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    test al, al                               ; 84 c0
    je short 045d8h                           ; 74 05
    mov dx, strict word 00002h                ; ba 02 00
    jmp short 045f2h                          ; eb 1a
    test dl, dl                               ; 84 d2
    je short 045e1h                           ; 74 05
    mov dx, strict word 00003h                ; ba 03 00
    jmp short 045f2h                          ; eb 11
    test bl, 080h                             ; f6 c3 80
    jne short 045eah                          ; 75 04
    xor dh, dh                                ; 30 f6
    jmp short 045f2h                          ; eb 08
    test bl, 080h                             ; f6 c3 80
    je short 04610h                           ; 74 21
    mov dx, strict word 00001h                ; ba 01 00
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 cf d2
    imul dx, dx, strict byte 0000ah           ; 6b d2 0a
    add dx, 00cfeh                            ; 81 c2 fe 0c
    push dx                                   ; 52
    push 00404h                               ; 68 04 04
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 00 d3
    add sp, strict byte 00006h                ; 83 c4 06
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
print_boot_failure_:                         ; 0xf4616 LB 0x93
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    mov dh, cl                                ; 88 ce
    mov ah, bl                                ; 88 dc
    and ah, 07fh                              ; 80 e4 7f
    movzx si, ah                              ; 0f b6 f4
    test al, al                               ; 84 c0
    je short 04643h                           ; 74 1b
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 99 d2
    push 00d12h                               ; 68 12 0d
    push 00418h                               ; 68 18 04
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 cf d2
    add sp, strict byte 00006h                ; 83 c4 06
    jmp short 04687h                          ; eb 44
    test dl, dl                               ; 84 d2
    je short 04657h                           ; 74 10
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 7a d2
    push 00d1ch                               ; 68 1c 0d
    jmp short 04636h                          ; eb df
    test bl, 080h                             ; f6 c3 80
    je short 0466dh                           ; 74 11
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 65 d2
    push si                                   ; 56
    push 00d08h                               ; 68 08 0d
    jmp short 0467ch                          ; eb 0f
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 54 d2
    push si                                   ; 56
    push 00cfeh                               ; 68 fe 0c
    push 0042dh                               ; 68 2d 04
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 89 d2
    add sp, strict byte 00008h                ; 83 c4 08
    cmp byte [bp+004h], 001h                  ; 80 7e 04 01
    jne short 046a1h                          ; 75 14
    test dh, dh                               ; 84 f6
    jne short 04696h                          ; 75 05
    push 00445h                               ; 68 45 04
    jmp short 04699h                          ; eb 03
    push 0046fh                               ; 68 6f 04
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 6f d2
    add sp, strict byte 00004h                ; 83 c4 04
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00002h                               ; c2 02 00
print_cdromboot_failure_:                    ; 0xf46a9 LB 0x27
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    mov dx, ax                                ; 89 c2
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 10 d2
    push dx                                   ; 52
    push 004a4h                               ; 68 a4 04
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 48 d2
    add sp, strict byte 00006h                ; 83 c4 06
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
_int19_function:                             ; 0xf46d0 LB 0x256
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000eh                ; 83 ec 0e
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 3b cf
    mov bx, ax                                ; 89 c3
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    mov ax, strict word 0003dh                ; b8 3d 00
    call 0165ch                               ; e8 6c cf
    movzx si, al                              ; 0f b6 f0
    mov ax, strict word 00038h                ; b8 38 00
    call 0165ch                               ; e8 63 cf
    and AL, strict byte 0f0h                  ; 24 f0
    xor ah, ah                                ; 30 e4
    sal ax, 004h                              ; c1 e0 04
    or si, ax                                 ; 09 c6
    mov ax, strict word 0003ch                ; b8 3c 00
    call 0165ch                               ; e8 54 cf
    and AL, strict byte 00fh                  ; 24 0f
    xor ah, ah                                ; 30 e4
    sal ax, 00ch                              ; c1 e0 0c
    or si, ax                                 ; 09 c6
    mov dx, 00339h                            ; ba 39 03
    mov ax, bx                                ; 89 d8
    call 01600h                               ; e8 e7 ce
    test al, al                               ; 84 c0
    je short 04728h                           ; 74 0b
    mov dx, 00339h                            ; ba 39 03
    mov ax, bx                                ; 89 d8
    call 01600h                               ; e8 db ce
    movzx si, al                              ; 0f b6 f0
    cmp byte [bp+004h], 001h                  ; 80 7e 04 01
    jne short 0473eh                          ; 75 10
    mov ax, strict word 0003ch                ; b8 3c 00
    call 0165ch                               ; e8 28 cf
    and AL, strict byte 0f0h                  ; 24 f0
    xor ah, ah                                ; 30 e4
    sar ax, 004h                              ; c1 f8 04
    call 07316h                               ; e8 d8 2b
    cmp byte [bp+004h], 002h                  ; 80 7e 04 02
    jne short 04747h                          ; 75 03
    shr si, 004h                              ; c1 ee 04
    cmp byte [bp+004h], 003h                  ; 80 7e 04 03
    jne short 04750h                          ; 75 03
    shr si, 008h                              ; c1 ee 08
    cmp byte [bp+004h], 004h                  ; 80 7e 04 04
    jne short 04759h                          ; 75 03
    shr si, 00ch                              ; c1 ee 0c
    cmp si, strict byte 00010h                ; 83 fe 10
    jnc short 04762h                          ; 73 04
    mov byte [bp-008h], 001h                  ; c6 46 f8 01
    xor al, al                                ; 30 c0
    mov byte [bp-006h], al                    ; 88 46 fa
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 54 d1
    push si                                   ; 56
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    push ax                                   ; 50
    push 004c4h                               ; 68 c4 04
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 87 d1
    add sp, strict byte 00008h                ; 83 c4 08
    and si, strict byte 0000fh                ; 83 e6 0f
    cmp si, strict byte 00002h                ; 83 fe 02
    jc short 0479fh                           ; 72 0e
    jbe short 047aeh                          ; 76 1b
    cmp si, strict byte 00004h                ; 83 fe 04
    je short 047cch                           ; 74 34
    cmp si, strict byte 00003h                ; 83 fe 03
    je short 047c2h                           ; 74 25
    jmp short 047f9h                          ; eb 5a
    cmp si, strict byte 00001h                ; 83 fe 01
    jne short 047f9h                          ; 75 55
    xor al, al                                ; 30 c0
    mov byte [bp-006h], al                    ; 88 46 fa
    mov byte [bp-00ah], al                    ; 88 46 f6
    jmp short 04811h                          ; eb 63
    mov dx, 00338h                            ; ba 38 03
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    call 01600h                               ; e8 49 ce
    add AL, strict byte 080h                  ; 04 80
    mov byte [bp-006h], al                    ; 88 46 fa
    mov byte [bp-00ah], 000h                  ; c6 46 f6 00
    jmp short 04811h                          ; eb 4f
    mov byte [bp-006h], 000h                  ; c6 46 fa 00
    mov byte [bp-00ah], 001h                  ; c6 46 f6 01
    jmp short 047d6h                          ; eb 0a
    mov byte [bp-00ch], 001h                  ; c6 46 f4 01
    cmp byte [bp-00ah], 000h                  ; 80 7e f6 00
    je short 04811h                           ; 74 3b
    call 037f6h                               ; e8 1d f0
    mov bx, ax                                ; 89 c3
    test AL, strict byte 0ffh                 ; a8 ff
    je short 04800h                           ; 74 21
    call 046a9h                               ; e8 c7 fe
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    mov cx, strict word 00001h                ; b9 01 00
    call 04616h                               ; e8 1d fe
    xor ax, ax                                ; 31 c0
    xor dx, dx                                ; 31 d2
    jmp near 0491fh                           ; e9 1f 01
    mov dx, 0032eh                            ; ba 2e 03
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    call 0161ch                               ; e8 13 ce
    mov di, ax                                ; 89 c7
    shr bx, 008h                              ; c1 eb 08
    mov byte [bp-006h], bl                    ; 88 5e fa
    cmp byte [bp-00ch], 001h                  ; 80 7e f4 01
    jne near 0488dh                           ; 0f 85 74 00
    xor si, si                                ; 31 f6
    mov ax, 0e200h                            ; b8 00 e2
    mov es, ax                                ; 8e c0
    cmp word [es:si], 0aa55h                  ; 26 81 3c 55 aa
    jne short 047e2h                          ; 75 bb
    mov cx, ax                                ; 89 c1
    mov si, word [es:si+01ah]                 ; 26 8b 74 1a
    cmp word [es:si+002h], 0506eh             ; 26 81 7c 02 6e 50
    jne short 047e2h                          ; 75 ad
    cmp word [es:si], 05024h                  ; 26 81 3c 24 50
    jne short 047e2h                          ; 75 a6
    mov di, word [es:si+00eh]                 ; 26 8b 7c 0e
    mov dx, word [es:di]                      ; 26 8b 15
    mov ax, word [es:di+002h]                 ; 26 8b 45 02
    cmp ax, 06568h                            ; 3d 68 65
    jne short 0486bh                          ; 75 1f
    cmp dx, 07445h                            ; 81 fa 45 74
    jne short 0486bh                          ; 75 19
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    call 045cbh                               ; e8 6a fd
    mov word [bp-012h], strict word 00006h    ; c7 46 ee 06 00
    mov word [bp-010h], cx                    ; 89 4e f0
    jmp short 04887h                          ; eb 1c
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    call 045cbh                               ; e8 51 fd
    sti                                       ; fb
    mov word [bp-010h], cx                    ; 89 4e f0
    mov es, cx                                ; 8e c1
    mov ax, word [es:si+01ah]                 ; 26 8b 44 1a
    mov word [bp-012h], ax                    ; 89 46 ee
    call far [bp-012h]                        ; ff 5e ee
    jmp near 047e2h                           ; e9 55 ff
    cmp byte [bp-00ah], 000h                  ; 80 7e f6 00
    jne short 048b9h                          ; 75 26
    cmp byte [bp-00ch], 000h                  ; 80 7e f4 00
    jne short 048b9h                          ; 75 20
    mov di, 007c0h                            ; bf c0 07
    mov es, di                                ; 8e c7
    mov dl, byte [bp-006h]                    ; 8a 56 fa
    mov ax, 00201h                            ; b8 01 02
    mov DH, strict byte 000h                  ; b6 00
    mov cx, strict word 00001h                ; b9 01 00
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    int 013h                                  ; cd 13
    mov ax, strict word 00000h                ; b8 00 00
    sbb ax, strict byte 00000h                ; 83 d8 00
    test ax, ax                               ; 85 c0
    jne near 047e2h                           ; 0f 85 29 ff
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    db  00fh, 094h, 0c1h
    ; sete cl                                   ; 0f 94 c1
    cmp byte [bp-00ah], 000h                  ; 80 7e f6 00
    je short 048c8h                           ; 74 02
    mov CL, strict byte 001h                  ; b1 01
    xor dx, dx                                ; 31 d2
    mov ax, di                                ; 89 f8
    call 0161ch                               ; e8 4d cd
    mov bx, ax                                ; 89 c3
    mov dx, strict word 00002h                ; ba 02 00
    mov ax, di                                ; 89 f8
    call 0161ch                               ; e8 43 cd
    cmp bx, ax                                ; 39 c3
    je short 048eeh                           ; 74 11
    test cl, cl                               ; 84 c9
    jne short 04904h                          ; 75 23
    mov dx, 001feh                            ; ba fe 01
    mov ax, di                                ; 89 f8
    call 0161ch                               ; e8 33 cd
    cmp ax, 0aa55h                            ; 3d 55 aa
    je short 04904h                           ; 74 16
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    xor cx, cx                                ; 31 c9
    jmp near 047f6h                           ; e9 f2 fe
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    call 045cbh                               ; e8 b8 fc
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    xor dx, dx                                ; 31 d2
    xor ax, ax                                ; 31 c0
    add ax, di                                ; 01 f8
    adc dx, bx                                ; 11 da
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
keyboard_panic_:                             ; 0xf4926 LB 0x13
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push ax                                   ; 50
    push 004e4h                               ; 68 e4 04
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 db cf
    add sp, strict byte 00006h                ; 83 c4 06
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
_keyboard_init:                              ; 0xf4939 LB 0x27c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov AL, strict byte 0aah                  ; b0 aa
    mov dx, strict word 00064h                ; ba 64 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 0495ch                           ; 74 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 0495ch                          ; 76 08
    xor al, al                                ; 30 c0
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04945h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04965h                          ; 75 05
    xor ax, ax                                ; 31 c0
    call 04926h                               ; e8 c1 ff
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 0497fh                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 0497fh                          ; 76 08
    mov AL, strict byte 001h                  ; b0 01
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04968h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04989h                          ; 75 06
    mov ax, strict word 00001h                ; b8 01 00
    call 04926h                               ; e8 9d ff
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, strict word 00055h                ; 3d 55 00
    je short 0499ah                           ; 74 06
    mov ax, 003dfh                            ; b8 df 03
    call 04926h                               ; e8 8c ff
    mov AL, strict byte 0abh                  ; b0 ab
    mov dx, strict word 00064h                ; ba 64 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 049bah                           ; 74 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 049bah                          ; 76 08
    mov AL, strict byte 010h                  ; b0 10
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 049a3h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 049c4h                          ; 75 06
    mov ax, strict word 0000ah                ; b8 0a 00
    call 04926h                               ; e8 62 ff
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 049deh                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 049deh                          ; 76 08
    mov AL, strict byte 011h                  ; b0 11
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 049c7h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 049e8h                          ; 75 06
    mov ax, strict word 0000bh                ; b8 0b 00
    call 04926h                               ; e8 3e ff
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test ax, ax                               ; 85 c0
    je short 049f8h                           ; 74 06
    mov ax, 003e0h                            ; b8 e0 03
    call 04926h                               ; e8 2e ff
    mov AL, strict byte 0ffh                  ; b0 ff
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 04a18h                           ; 74 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04a18h                          ; 76 08
    mov AL, strict byte 020h                  ; b0 20
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04a01h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04a22h                          ; 75 06
    mov ax, strict word 00014h                ; b8 14 00
    call 04926h                               ; e8 04 ff
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 04a3ch                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04a3ch                          ; 76 08
    mov AL, strict byte 021h                  ; b0 21
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04a25h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04a46h                          ; 75 06
    mov ax, strict word 00015h                ; b8 15 00
    call 04926h                               ; e8 e0 fe
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, 000fah                            ; 3d fa 00
    je short 04a57h                           ; 74 06
    mov ax, 003e1h                            ; b8 e1 03
    call 04926h                               ; e8 cf fe
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 04a71h                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04a71h                          ; 76 08
    mov AL, strict byte 031h                  ; b0 31
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04a5ah                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04a7bh                          ; 75 06
    mov ax, strict word 0001fh                ; b8 1f 00
    call 04926h                               ; e8 ab fe
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, 000aah                            ; 3d aa 00
    je short 04a94h                           ; 74 0e
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, 000aah                            ; 3d aa 00
    je short 04a94h                           ; 74 06
    mov ax, 003e2h                            ; b8 e2 03
    call 04926h                               ; e8 92 fe
    mov AL, strict byte 0f5h                  ; b0 f5
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 04ab4h                           ; 74 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04ab4h                          ; 76 08
    mov AL, strict byte 040h                  ; b0 40
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04a9dh                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04abeh                          ; 75 06
    mov ax, strict word 00028h                ; b8 28 00
    call 04926h                               ; e8 68 fe
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 04ad8h                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04ad8h                          ; 76 08
    mov AL, strict byte 041h                  ; b0 41
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04ac1h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04ae2h                          ; 75 06
    mov ax, strict word 00029h                ; b8 29 00
    call 04926h                               ; e8 44 fe
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, 000fah                            ; 3d fa 00
    je short 04af3h                           ; 74 06
    mov ax, 003e3h                            ; b8 e3 03
    call 04926h                               ; e8 33 fe
    mov AL, strict byte 060h                  ; b0 60
    mov dx, strict word 00064h                ; ba 64 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 04b13h                           ; 74 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04b13h                          ; 76 08
    mov AL, strict byte 050h                  ; b0 50
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04afch                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04b1dh                          ; 75 06
    mov ax, strict word 00032h                ; b8 32 00
    call 04926h                               ; e8 09 fe
    mov AL, strict byte 065h                  ; b0 65
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 04b3dh                           ; 74 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04b3dh                          ; 76 08
    mov AL, strict byte 060h                  ; b0 60
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04b26h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04b47h                          ; 75 06
    mov ax, strict word 0003ch                ; b8 3c 00
    call 04926h                               ; e8 df fd
    mov AL, strict byte 0f4h                  ; b0 f4
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 04b67h                           ; 74 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04b67h                          ; 76 08
    mov AL, strict byte 070h                  ; b0 70
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04b50h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04b71h                          ; 75 06
    mov ax, strict word 00046h                ; b8 46 00
    call 04926h                               ; e8 b5 fd
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 04b8bh                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 04b8bh                          ; 76 08
    mov AL, strict byte 071h                  ; b0 71
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 04b74h                          ; eb e9
    test bx, bx                               ; 85 db
    jne short 04b95h                          ; 75 06
    mov ax, strict word 00046h                ; b8 46 00
    call 04926h                               ; e8 91 fd
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, 000fah                            ; 3d fa 00
    je short 04ba6h                           ; 74 06
    mov ax, 003e4h                            ; b8 e4 03
    call 04926h                               ; e8 80 fd
    mov AL, strict byte 0a8h                  ; b0 a8
    mov dx, strict word 00064h                ; ba 64 00
    out DX, AL                                ; ee
    xor ax, ax                                ; 31 c0
    call 05db4h                               ; e8 03 12
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
enqueue_key_:                                ; 0xf4bb5 LB 0x93
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov bl, dl                                ; 88 d3
    mov dx, strict word 0001ah                ; ba 1a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 51 ca
    mov di, ax                                ; 89 c7
    mov dx, strict word 0001ch                ; ba 1c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 46 ca
    mov si, ax                                ; 89 c6
    lea cx, [si+002h]                         ; 8d 4c 02
    cmp cx, strict byte 0003eh                ; 83 f9 3e
    jc short 04be3h                           ; 72 03
    mov cx, strict word 0001eh                ; b9 1e 00
    cmp cx, di                                ; 39 f9
    jne short 04bebh                          ; 75 04
    xor ax, ax                                ; 31 c0
    jmp short 04c10h                          ; eb 25
    xor bh, bh                                ; 30 ff
    mov dx, si                                ; 89 f2
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 19 ca
    movzx bx, byte [bp-00ah]                  ; 0f b6 5e f6
    lea dx, [si+001h]                         ; 8d 54 01
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 0c ca
    mov bx, cx                                ; 89 cb
    mov dx, strict word 0001ch                ; ba 1c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0162ah                               ; e8 1d ca
    mov ax, strict word 00001h                ; b8 01 00
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
    db  0c6h, 0c5h, 0bah
    ; mov ch, 0bah                              ; c6 c5 ba
    mov ax, 0aab6h                            ; b8 b6 aa
    popfw                                     ; 9d
    push bx                                   ; 53
    inc si                                    ; 46
    inc bp                                    ; 45
    cmp bh, byte [bx+si]                      ; 3a 38
    sub bl, byte [ss:di]                      ; 36 2a 1d
    db  082h, 04eh, 00fh, 04dh
    ; or byte [bp+00fh], 04dh                   ; 82 4e 0f 4d
    fimul word [si-022h]                      ; de 4c de
    dec sp                                    ; 4c
    xchg cx, ax                               ; 91
    dec bp                                    ; 4d
    mov BH, strict byte 04ch                  ; b7 4c
    add cx, word [bp+052h]                    ; 03 4e 52
    dec si                                    ; 4e
    jne short 04c88h                          ; 75 4e
    push ax                                   ; 50
    dec bp                                    ; 4d
    fimul word [si-022h]                      ; de 4c de
    dec sp                                    ; 4c
    retf 0d04dh                               ; ca 4d d0
    dec sp                                    ; 4c
    xor cx, word [bp+06eh]                    ; 33 4e 6e
    dec si                                    ; 4e
_int09_function:                             ; 0xf4c48 LB 0x35d
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ch                ; 83 ec 0c
    mov al, byte [bp+014h]                    ; 8a 46 14
    mov byte [bp-00ah], al                    ; 88 46 f6
    test al, al                               ; 84 c0
    jne short 04c73h                          ; 75 19
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 67 cc
    push 004f7h                               ; 68 f7 04
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 a0 cc
    add sp, strict byte 00004h                ; 83 c4 04
    jmp near 04f9eh                           ; e9 2b 03
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 84 c9
    mov byte [bp-00eh], al                    ; 88 46 f2
    mov bl, al                                ; 88 c3
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 76 c9
    mov byte [bp-010h], al                    ; 88 46 f0
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov dx, 00096h                            ; ba 96 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 67 c9
    mov byte [bp-008h], al                    ; 88 46 f8
    mov byte [bp-006h], al                    ; 88 46 fa
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 00010h                ; b9 10 00
    mov di, 04c19h                            ; bf 19 4c
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov ax, word [cs:di+04c28h]               ; 2e 8b 85 28 4c
    jmp ax                                    ; ff e0
    xor bl, 040h                              ; 80 f3 40
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 49 c9
    or byte [bp-00ch], 040h                   ; 80 4e f4 40
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    jmp near 04e46h                           ; e9 76 01
    mov al, byte [bp-010h]                    ; 8a 46 f0
    and AL, strict byte 0bfh                  ; 24 bf
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    jmp near 04e46h                           ; e9 68 01
    test byte [bp-006h], 002h                 ; f6 46 fa 02
    jne near 04f80h                           ; 0f 85 9a 02
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    and AL, strict byte 07fh                  ; 24 7f
    cmp AL, strict byte 02ah                  ; 3c 2a
    db  00fh, 094h, 0c0h
    ; sete al                                   ; 0f 94 c0
    xor ah, ah                                ; 30 e4
    inc ax                                    ; 40
    test byte [bp-00ah], 080h                 ; f6 46 f6 80
    je short 04cffh                           ; 74 06
    not al                                    ; f6 d0
    and bl, al                                ; 20 c3
    jmp short 04d01h                          ; eb 02
    or bl, al                                 ; 08 c3
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 02 c9
    jmp near 04f80h                           ; e9 71 02
    test byte [bp-008h], 001h                 ; f6 46 f8 01
    jne near 04f80h                           ; 0f 85 69 02
    or bl, 004h                               ; 80 cb 04
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 e9 c8
    mov al, byte [bp-008h]                    ; 8a 46 f8
    test AL, strict byte 002h                 ; a8 02
    je short 04d39h                           ; 74 0d
    or AL, strict byte 004h                   ; 0c 04
    mov byte [bp-006h], al                    ; 88 46 fa
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00096h                            ; ba 96 00
    jmp short 04d47h                          ; eb 0e
    mov al, byte [bp-010h]                    ; 8a 46 f0
    or AL, strict byte 001h                   ; 0c 01
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 c1 c8
    jmp near 04f80h                           ; e9 30 02
    test byte [bp-008h], 001h                 ; f6 46 f8 01
    jne near 04f80h                           ; 0f 85 28 02
    and bl, 0fbh                              ; 80 e3 fb
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 a8 c8
    mov al, byte [bp-008h]                    ; 8a 46 f8
    test AL, strict byte 002h                 ; a8 02
    je short 04d7ah                           ; 74 0d
    and AL, strict byte 0fbh                  ; 24 fb
    mov byte [bp-006h], al                    ; 88 46 fa
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00096h                            ; ba 96 00
    jmp short 04d88h                          ; eb 0e
    mov al, byte [bp-010h]                    ; 8a 46 f0
    and AL, strict byte 0feh                  ; 24 fe
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 80 c8
    jmp near 04f80h                           ; e9 ef 01
    or bl, 008h                               ; 80 cb 08
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 6f c8
    mov al, byte [bp-008h]                    ; 8a 46 f8
    test AL, strict byte 002h                 ; a8 02
    je short 04db3h                           ; 74 0d
    or AL, strict byte 008h                   ; 0c 08
    mov byte [bp-006h], al                    ; 88 46 fa
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00096h                            ; ba 96 00
    jmp short 04dc1h                          ; eb 0e
    mov al, byte [bp-010h]                    ; 8a 46 f0
    or AL, strict byte 002h                   ; 0c 02
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 47 c8
    jmp near 04f80h                           ; e9 b6 01
    and bl, 0f7h                              ; 80 e3 f7
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 36 c8
    mov al, byte [bp-008h]                    ; 8a 46 f8
    test AL, strict byte 002h                 ; a8 02
    je short 04dech                           ; 74 0d
    and AL, strict byte 0f7h                  ; 24 f7
    mov byte [bp-006h], al                    ; 88 46 fa
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00096h                            ; ba 96 00
    jmp short 04dfah                          ; eb 0e
    mov al, byte [bp-010h]                    ; 8a 46 f0
    and AL, strict byte 0fdh                  ; 24 fd
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 0e c8
    jmp near 04f80h                           ; e9 7d 01
    test byte [bp-008h], 003h                 ; f6 46 f8 03
    jne near 04f80h                           ; 0f 85 75 01
    mov al, byte [bp-010h]                    ; 8a 46 f0
    or AL, strict byte 020h                   ; 0c 20
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 ef c7
    mov bl, byte [bp-00eh]                    ; 8a 5e f2
    xor bl, 020h                              ; 80 f3 20
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 de c7
    jmp near 04f80h                           ; e9 4d 01
    test byte [bp-008h], 003h                 ; f6 46 f8 03
    jne near 04f80h                           ; 0f 85 45 01
    mov al, byte [bp-010h]                    ; 8a 46 f0
    and AL, strict byte 0dfh                  ; 24 df
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 bf c7
    jmp near 04f80h                           ; e9 2e 01
    mov al, byte [bp-010h]                    ; 8a 46 f0
    or AL, strict byte 010h                   ; 0c 10
    mov byte [bp-00ch], al                    ; 88 46 f4
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 a8 c7
    mov bl, byte [bp-00eh]                    ; 8a 5e f2
    xor bl, 010h                              ; 80 f3 10
    jmp short 04e25h                          ; eb b7
    mov al, byte [bp-010h]                    ; 8a 46 f0
    and AL, strict byte 0efh                  ; 24 ef
    jmp short 04e40h                          ; eb cb
    mov al, bl                                ; 88 d8
    and AL, strict byte 00ch                  ; 24 0c
    cmp AL, strict byte 00ch                  ; 3c 0c
    jne short 04e82h                          ; 75 05
    jmp far 0f000h:0e05bh                     ; ea 5b e0 00 f0
    test byte [bp-00ah], 080h                 ; f6 46 f6 80
    jne near 04f80h                           ; 0f 85 f6 00
    cmp byte [bp-00ah], 058h                  ; 80 7e f6 58
    jbe short 04eaeh                          ; 76 1e
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 31 ca
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    push 00511h                               ; 68 11 05
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 65 ca
    add sp, strict byte 00006h                ; 83 c4 06
    jmp near 04f9eh                           ; e9 f0 00
    test bl, 008h                             ; f6 c3 08
    je short 04ec5h                           ; 74 12
    movzx si, byte [bp-00ah]                  ; 0f b6 76 f6
    imul si, si, strict byte 0000ah           ; 6b f6 0a
    mov dl, byte [si+00d2ch]                  ; 8a 94 2c 0d
    mov ax, word [si+00d2ch]                  ; 8b 84 2c 0d
    jmp near 04f51h                           ; e9 8c 00
    test bl, 004h                             ; f6 c3 04
    je short 04edch                           ; 74 12
    movzx si, byte [bp-00ah]                  ; 0f b6 76 f6
    imul si, si, strict byte 0000ah           ; 6b f6 0a
    mov dl, byte [si+00d2ah]                  ; 8a 94 2a 0d
    mov ax, word [si+00d2ah]                  ; 8b 84 2a 0d
    jmp near 04f51h                           ; e9 75 00
    mov al, byte [bp-006h]                    ; 8a 46 fa
    and AL, strict byte 002h                  ; 24 02
    test al, al                               ; 84 c0
    jbe short 04efah                          ; 76 15
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    cmp AL, strict byte 047h                  ; 3c 47
    jc short 04efah                           ; 72 0e
    cmp AL, strict byte 053h                  ; 3c 53
    jnbe short 04efah                         ; 77 0a
    mov DL, strict byte 0e0h                  ; b2 e0
    movzx si, al                              ; 0f b6 f0
    imul si, si, strict byte 0000ah           ; 6b f6 0a
    jmp short 04f4dh                          ; eb 53
    test bl, 003h                             ; f6 c3 03
    je short 04f2ch                           ; 74 2d
    movzx si, byte [bp-00ah]                  ; 0f b6 76 f6
    imul si, si, strict byte 0000ah           ; 6b f6 0a
    movzx ax, byte [si+00d2eh]                ; 0f b6 84 2e 0d
    movzx dx, bl                              ; 0f b6 d3
    test dx, ax                               ; 85 c2
    je short 04f1ch                           ; 74 0a
    mov dl, byte [si+00d26h]                  ; 8a 94 26 0d
    mov ax, word [si+00d26h]                  ; 8b 84 26 0d
    jmp short 04f24h                          ; eb 08
    mov dl, byte [si+00d28h]                  ; 8a 94 28 0d
    mov ax, word [si+00d28h]                  ; 8b 84 28 0d
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp-00ah], al                    ; 88 46 f6
    jmp short 04f57h                          ; eb 2b
    movzx si, byte [bp-00ah]                  ; 0f b6 76 f6
    imul si, si, strict byte 0000ah           ; 6b f6 0a
    movzx ax, byte [si+00d2eh]                ; 0f b6 84 2e 0d
    movzx dx, bl                              ; 0f b6 d3
    test dx, ax                               ; 85 c2
    je short 04f49h                           ; 74 0a
    mov dl, byte [si+00d28h]                  ; 8a 94 28 0d
    mov ax, word [si+00d28h]                  ; 8b 84 28 0d
    jmp short 04f51h                          ; eb 08
    mov dl, byte [si+00d26h]                  ; 8a 94 26 0d
    mov ax, word [si+00d26h]                  ; 8b 84 26 0d
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp-00ah], al                    ; 88 46 f6
    cmp byte [bp-00ah], 000h                  ; 80 7e f6 00
    jne short 04f77h                          ; 75 1a
    test dl, dl                               ; 84 d2
    jne short 04f77h                          ; 75 16
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 60 c9
    push 00548h                               ; 68 48 05
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 99 c9
    add sp, strict byte 00004h                ; 83 c4 04
    xor dh, dh                                ; 30 f6
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    call 04bb5h                               ; e8 35 fc
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    and AL, strict byte 07fh                  ; 24 7f
    cmp AL, strict byte 01dh                  ; 3c 1d
    je short 04f8dh                           ; 74 04
    and byte [bp-006h], 0feh                  ; 80 66 fa fe
    and byte [bp-006h], 0fdh                  ; 80 66 fa fd
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    mov dx, 00096h                            ; ba 96 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 70 c6
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
dequeue_key_:                                ; 0xf4fa5 LB 0x94
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov di, ax                                ; 89 c7
    mov word [bp-006h], dx                    ; 89 56 fa
    mov si, bx                                ; 89 de
    mov word [bp-008h], cx                    ; 89 4e f8
    mov dx, strict word 0001ah                ; ba 1a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 5d c6
    mov bx, ax                                ; 89 c3
    mov dx, strict word 0001ch                ; ba 1c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 52 c6
    cmp bx, ax                                ; 39 c3
    je short 0500bh                           ; 74 3d
    mov dx, bx                                ; 89 da
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 2a c6
    mov cl, al                                ; 88 c1
    lea dx, [bx+001h]                         ; 8d 57 01
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 1f c6
    mov es, [bp-008h]                         ; 8e 46 f8
    mov byte [es:si], cl                      ; 26 88 0c
    mov es, [bp-006h]                         ; 8e 46 fa
    mov byte [es:di], al                      ; 26 88 05
    cmp word [bp+004h], strict byte 00000h    ; 83 7e 04 00
    je short 05006h                           ; 74 13
    inc bx                                    ; 43
    inc bx                                    ; 43
    cmp bx, strict byte 0003eh                ; 83 fb 3e
    jc short 04ffdh                           ; 72 03
    mov bx, strict word 0001eh                ; bb 1e 00
    mov dx, strict word 0001ah                ; ba 1a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0162ah                               ; e8 24 c6
    mov ax, strict word 00001h                ; b8 01 00
    jmp short 0500dh                          ; eb 02
    xor ax, ax                                ; 31 c0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00002h                               ; c2 02 00
    mov byte [01292h], AL                     ; a2 92 12
    adc word [bx+si], dx                      ; 11 10
    or cl, byte [bx+di]                       ; 0a 09
    add ax, 00102h                            ; 05 02 01
    add bh, dl                                ; 00 d7
    push dx                                   ; 52
    in ax, DX                                 ; ed
    push ax                                   ; 50
    xor AL, strict byte 051h                  ; 34 51
    adc byte [bx+di-070h], 051h               ; 80 51 90 51
    mov dx, 0c351h                            ; ba 51 c3
    push cx                                   ; 51
    xor AL, strict byte 052h                  ; 34 52
    db  065h, 052h
    ; gs push dx                                ; 65 52
    xchg dx, ax                               ; 92
    push dx                                   ; 52
    int3                                      ; cc
    push dx                                   ; 52
    db  01ah
    push bx                                   ; 53
_int16_function:                             ; 0xf5039 LB 0x319
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 b7 c5
    mov cl, al                                ; 88 c1
    mov bh, al                                ; 88 c7
    mov dx, 00097h                            ; ba 97 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 aa c5
    mov bl, al                                ; 88 c3
    movzx dx, cl                              ; 0f b6 d1
    sar dx, 004h                              ; c1 fa 04
    and dl, 007h                              ; 80 e2 07
    and AL, strict byte 007h                  ; 24 07
    xor ah, ah                                ; 30 e4
    xor al, dl                                ; 30 d0
    test ax, ax                               ; 85 c0
    je short 050cbh                           ; 74 60
    cli                                       ; fa
    mov AL, strict byte 0edh                  ; b0 ed
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 05084h                          ; 75 08
    mov AL, strict byte 021h                  ; b0 21
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 05072h                          ; eb ee
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, 000fah                            ; 3d fa 00
    jne short 050cah                          ; 75 3b
    and bl, 0f8h                              ; 80 e3 f8
    movzx ax, bh                              ; 0f b6 c7
    sar ax, 004h                              ; c1 f8 04
    and ax, strict word 00007h                ; 25 07 00
    movzx cx, bl                              ; 0f b6 cb
    or cx, ax                                 ; 09 c1
    mov bl, cl                                ; 88 cb
    mov al, cl                                ; 88 c8
    and AL, strict byte 007h                  ; 24 07
    out DX, AL                                ; ee
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 050b9h                          ; 75 08
    mov AL, strict byte 021h                  ; b0 21
    mov dx, 00080h                            ; ba 80 00
    out DX, AL                                ; ee
    jmp short 050a7h                          ; eb ee
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    xor bh, bh                                ; 30 ff
    mov dx, 00097h                            ; ba 97 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 44 c5
    sti                                       ; fb
    mov ax, word [bp+012h]                    ; 8b 46 12
    shr ax, 008h                              ; c1 e8 08
    cmp ax, 000a2h                            ; 3d a2 00
    jnbe near 052d7h                          ; 0f 87 ff 01
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 0000ch                ; b9 0c 00
    mov di, 05016h                            ; bf 16 50
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov ax, word [cs:di+05021h]               ; 2e 8b 85 21 50
    jmp ax                                    ; ff e0
    push strict byte 00001h                   ; 6a 01
    mov cx, ss                                ; 8c d1
    lea bx, [bp-008h]                         ; 8d 5e f8
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 04fa5h                               ; e8 a9 fe
    test ax, ax                               ; 85 c0
    jne short 0510bh                          ; 75 0b
    push 0057fh                               ; 68 7f 05
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 05 c8
    add sp, strict byte 00004h                ; 83 c4 04
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    je short 05117h                           ; 74 06
    cmp byte [bp-008h], 0f0h                  ; 80 7e f8 f0
    je short 0511dh                           ; 74 06
    cmp byte [bp-008h], 0e0h                  ; 80 7e f8 e0
    jne short 05121h                          ; 75 04
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    sal dx, 008h                              ; c1 e2 08
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    or dx, ax                                 ; 09 c2
    mov word [bp+012h], dx                    ; 89 56 12
    jmp near 0531ah                           ; e9 e6 01
    or word [bp+01ch], 00200h                 ; 81 4e 1c 00 02
    push strict byte 00000h                   ; 6a 00
    mov cx, ss                                ; 8c d1
    lea bx, [bp-008h]                         ; 8d 5e f8
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 04fa5h                               ; e8 5d fe
    test ax, ax                               ; 85 c0
    jne short 05153h                          ; 75 07
    or word [bp+01ch], strict byte 00040h     ; 83 4e 1c 40
    jmp near 0531ah                           ; e9 c7 01
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    je short 0515fh                           ; 74 06
    cmp byte [bp-008h], 0f0h                  ; 80 7e f8 f0
    je short 05165h                           ; 74 06
    cmp byte [bp-008h], 0e0h                  ; 80 7e f8 e0
    jne short 05169h                          ; 75 04
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    movzx dx, byte [bp-006h]                  ; 0f b6 56 fa
    sal dx, 008h                              ; c1 e2 08
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    or dx, ax                                 ; 09 c2
    mov word [bp+012h], dx                    ; 89 56 12
    and word [bp+01ch], strict byte 0ffbfh    ; 83 66 1c bf
    jmp near 0531ah                           ; e9 9a 01
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 77 c4
    mov dx, word [bp+012h]                    ; 8b 56 12
    mov dl, al                                ; 88 c2
    jmp short 0512eh                          ; eb 9e
    mov al, byte [bp+010h]                    ; 8a 46 10
    movzx dx, al                              ; 0f b6 d0
    mov ax, word [bp+010h]                    ; 8b 46 10
    shr ax, 008h                              ; c1 e8 08
    xor ah, ah                                ; 30 e4
    call 04bb5h                               ; e8 14 fa
    test ax, ax                               ; 85 c0
    jne short 051b2h                          ; 75 0d
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 001h                   ; 0c 01
    mov word [bp+012h], ax                    ; 89 46 12
    jmp near 0531ah                           ; e9 68 01
    and word [bp+012h], 0ff00h                ; 81 66 12 00 ff
    jmp near 0531ah                           ; e9 60 01
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor al, al                                ; 30 c0
    or AL, strict byte 030h                   ; 0c 30
    jmp short 051ach                          ; eb e9
    mov byte [bp-004h], 002h                  ; c6 46 fc 02
    xor cx, cx                                ; 31 c9
    cli                                       ; fa
    mov AL, strict byte 0f2h                  ; b0 f2
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 051eah                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 051eah                          ; 76 08
    mov dx, 00080h                            ; ba 80 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    jmp short 051d3h                          ; eb e9
    test bx, bx                               ; 85 db
    jbe short 0522eh                          ; 76 40
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp ax, 000fah                            ; 3d fa 00
    jne short 0522eh                          ; 75 35
    mov bx, strict word 0ffffh                ; bb ff ff
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 05213h                          ; 75 0d
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 05213h                          ; 76 08
    mov dx, 00080h                            ; ba 80 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    jmp short 051fch                          ; eb e9
    test bx, bx                               ; 85 db
    jbe short 05225h                          ; 76 0e
    shr cx, 008h                              ; c1 e9 08
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    sal ax, 008h                              ; c1 e0 08
    or cx, ax                                 ; 09 c1
    dec byte [bp-004h]                        ; fe 4e fc
    cmp byte [bp-004h], 000h                  ; 80 7e fc 00
    jnbe short 051f9h                         ; 77 cb
    mov word [bp+00ch], cx                    ; 89 4e 0c
    jmp near 0531ah                           ; e9 e6 00
    push strict byte 00001h                   ; 6a 01
    mov cx, ss                                ; 8c d1
    lea bx, [bp-008h]                         ; 8d 5e f8
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 04fa5h                               ; e8 62 fd
    test ax, ax                               ; 85 c0
    jne short 05252h                          ; 75 0b
    push 0057fh                               ; 68 7f 05
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 be c6
    add sp, strict byte 00004h                ; 83 c4 04
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    je near 05121h                            ; 0f 84 c7 fe
    cmp byte [bp-008h], 0f0h                  ; 80 7e f8 f0
    je near 0511dh                            ; 0f 84 bb fe
    jmp near 05121h                           ; e9 bc fe
    or word [bp+01ch], 00200h                 ; 81 4e 1c 00 02
    push strict byte 00000h                   ; 6a 00
    mov cx, ss                                ; 8c d1
    lea bx, [bp-008h]                         ; 8d 5e f8
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 04fa5h                               ; e8 2c fd
    test ax, ax                               ; 85 c0
    je near 0514ch                            ; 0f 84 cd fe
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    je near 05169h                            ; 0f 84 e2 fe
    cmp byte [bp-008h], 0f0h                  ; 80 7e f8 f0
    je near 05165h                            ; 0f 84 d6 fe
    jmp near 05169h                           ; e9 d7 fe
    mov dx, strict word 00017h                ; ba 17 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 65 c3
    mov dx, word [bp+012h]                    ; 8b 56 12
    mov dl, al                                ; 88 c2
    mov word [bp+012h], dx                    ; 89 56 12
    mov dx, strict word 00018h                ; ba 18 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 54 c3
    mov bh, al                                ; 88 c7
    and bh, 073h                              ; 80 e7 73
    mov dx, 00096h                            ; ba 96 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 46 c3
    and AL, strict byte 00ch                  ; 24 0c
    or bh, al                                 ; 08 c7
    mov dx, word [bp+012h]                    ; 8b 56 12
    xor dh, dh                                ; 30 f6
    movzx ax, bh                              ; 0f b6 c7
    sal ax, 008h                              ; c1 e0 08
    jmp near 0512ch                           ; e9 60 fe
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    or ah, 080h                               ; 80 cc 80
    jmp near 051ach                           ; e9 d5 fe
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 ea c5
    mov ax, word [bp+012h]                    ; 8b 46 12
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005a3h                               ; 68 a3 05
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 1c c6
    add sp, strict byte 00006h                ; 83 c4 06
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 cd c5
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    push ax                                   ; 50
    mov ax, word [bp+010h]                    ; 8b 46 10
    push ax                                   ; 50
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    push ax                                   ; 50
    mov ax, word [bp+012h]                    ; 8b 46 12
    push ax                                   ; 50
    push 005cbh                               ; 68 cb 05
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 f6 c5
    add sp, strict byte 0000ch                ; 83 c4 0c
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop di                                    ; 5f
    pop bp                                    ; 5d
    retn                                      ; c3
    fcom qword [bp+di+002h]                   ; dc 53 02
    push sp                                   ; 54
    das                                       ; 2f
    push sp                                   ; 54
    das                                       ; 2f
    push sp                                   ; 54
    das                                       ; 2f
    push sp                                   ; 54
    pop ES                                    ; 07
    push si                                   ; 56
    xor ax, 03557h                            ; 35 57 35
    push di                                   ; 57
    and byte [bp+012h], dl                    ; 20 56 12
    push di                                   ; 57
    xor ax, 03557h                            ; 35 57 35
    push di                                   ; 57
    adc dl, byte [bx+012h]                    ; 12 57 12
    push di                                   ; 57
    xor ax, 03557h                            ; 35 57 35
    push di                                   ; 57
    xchg si, ax                               ; 96
    push si                                   ; 56
    adc dl, byte [bx+035h]                    ; 12 57 35
    push di                                   ; 57
    xor ax, 01257h                            ; 35 57 12
    push di                                   ; 57
    db  0c6h, 056h, 035h, 057h
    ; mov byte [bp+035h], 057h                  ; c6 56 35 57
    xor ax, 03557h                            ; 35 57 35
    push di                                   ; 57
_int13_harddisk:                             ; 0xf5352 LB 0x441
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    sub sp, strict byte 00010h                ; 83 ec 10
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 bb c2
    mov si, 00122h                            ; be 22 01
    mov word [bp-004h], ax                    ; 89 46 fc
    xor bx, bx                                ; 31 db
    mov dx, 0008eh                            ; ba 8e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 9c c2
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    xor ah, ah                                ; 30 e4
    cmp ax, 00080h                            ; 3d 80 00
    jc short 05381h                           ; 72 05
    cmp ax, 00090h                            ; 3d 90 00
    jc short 0539fh                           ; 72 1e
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005eeh                               ; 68 ee 05
    push 005fdh                               ; 68 fd 05
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 74 c5
    add sp, strict byte 0000ah                ; 83 c4 0a
    jmp near 05750h                           ; e9 b1 03
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    xor ah, ah                                ; 30 e4
    mov es, [bp-004h]                         ; 8e 46 fc
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov dl, byte [es:bx+0011fh]               ; 26 8a 97 1f 01
    mov byte [bp-002h], dl                    ; 88 56 fe
    cmp dl, 010h                              ; 80 fa 10
    jc short 053c8h                           ; 72 10
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005eeh                               ; 68 ee 05
    push 00628h                               ; 68 28 06
    jmp short 05394h                          ; eb cc
    mov bx, word [bp+016h]                    ; 8b 5e 16
    shr bx, 008h                              ; c1 eb 08
    cmp bx, strict byte 00018h                ; 83 fb 18
    jnbe near 05735h                          ; 0f 87 60 03
    add bx, bx                                ; 01 db
    jmp word [cs:bx+05320h]                   ; 2e ff a7 20 53
    cmp byte [bp-002h], 008h                  ; 80 7e fe 08
    jnc near 053ebh                           ; 0f 83 07 00
    movzx ax, byte [bp-002h]                  ; 0f b6 46 fe
    call 01c0ch                               ; e8 21 c8
    mov byte [bp+017h], 000h                  ; c6 46 17 00
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 14 c2
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 f5 c1
    mov cl, al                                ; 88 c1
    mov dx, word [bp+016h]                    ; 8b 56 16
    xor dh, dh                                ; 30 f6
    movzx ax, cl                              ; 0f b6 c1
    sal ax, 008h                              ; c1 e0 08
    or dx, ax                                 ; 09 c2
    mov word [bp+016h], dx                    ; 89 56 16
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 e6 c1
    test cl, cl                               ; 84 c9
    je short 053efh                           ; 74 c3
    jmp near 0576ch                           ; e9 3d 03
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov di, word [bp+014h]                    ; 8b 7e 14
    shr di, 008h                              ; c1 ef 08
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor ah, ah                                ; 30 e4
    sal ax, 002h                              ; c1 e0 02
    xor al, al                                ; 30 c0
    and ah, 003h                              ; 80 e4 03
    or di, ax                                 ; 09 c7
    mov ax, word [bp+014h]                    ; 8b 46 14
    and ax, strict word 0003fh                ; 25 3f 00
    mov word [bp-006h], ax                    ; 89 46 fa
    mov ax, word [bp+012h]                    ; 8b 46 12
    shr ax, 008h                              ; c1 e8 08
    mov word [bp-008h], ax                    ; 89 46 f8
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    cmp ax, 00080h                            ; 3d 80 00
    jnbe short 0546ah                         ; 77 04
    test ax, ax                               ; 85 c0
    jne short 0548dh                          ; 75 23
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 57 c4
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005eeh                               ; 68 ee 05
    push 0065ah                               ; 68 5a 06
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 86 c4
    add sp, strict byte 00008h                ; 83 c4 08
    jmp near 05750h                           ; e9 c3 02
    movzx ax, byte [bp-002h]                  ; 0f b6 46 fe
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-004h]                         ; 8e 46 fc
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov ax, word [es:bx+028h]                 ; 26 8b 47 28
    mov cx, word [es:bx+026h]                 ; 26 8b 4f 26
    mov dx, word [es:bx+02ah]                 ; 26 8b 57 2a
    mov word [bp-00ah], dx                    ; 89 56 f6
    cmp di, ax                                ; 39 c7
    jnc short 054bah                          ; 73 0c
    cmp cx, word [bp-008h]                    ; 3b 4e f8
    jbe short 054bah                          ; 76 07
    mov ax, word [bp-006h]                    ; 8b 46 fa
    cmp ax, dx                                ; 39 d0
    jbe short 054e8h                          ; 76 2e
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 07 c4
    push dword [bp-008h]                      ; 66 ff 76 f8
    push di                                   ; 57
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005eeh                               ; 68 ee 05
    push 00682h                               ; 68 82 06
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 2b c4
    add sp, strict byte 00010h                ; 83 c4 10
    jmp near 05750h                           ; e9 68 02
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    cmp ax, strict word 00004h                ; 3d 04 00
    jne short 054f6h                          ; 75 03
    jmp near 053ebh                           ; e9 f5 fe
    movzx bx, byte [bp-002h]                  ; 0f b6 5e fe
    imul bx, bx, strict byte 00018h           ; 6b db 18
    mov es, [bp-004h]                         ; 8e 46 fc
    add bx, si                                ; 01 f3
    cmp cx, word [es:bx+02ch]                 ; 26 3b 4f 2c
    jne short 05517h                          ; 75 0f
    mov ax, word [es:bx+030h]                 ; 26 8b 47 30
    cmp ax, word [bp-00ah]                    ; 3b 46 f6
    jne short 05517h                          ; 75 06
    cmp byte [bp-002h], 008h                  ; 80 7e fe 08
    jc short 05547h                           ; 72 30
    mov ax, di                                ; 89 f8
    xor dx, dx                                ; 31 d2
    mov bx, cx                                ; 89 cb
    xor cx, cx                                ; 31 c9
    call 08e19h                               ; e8 f7 38
    xor bx, bx                                ; 31 db
    add ax, word [bp-008h]                    ; 03 46 f8
    adc dx, bx                                ; 11 da
    mov bx, word [bp-00ah]                    ; 8b 5e f6
    xor cx, cx                                ; 31 c9
    call 08e19h                               ; e8 e8 38
    xor bx, bx                                ; 31 db
    add ax, word [bp-006h]                    ; 03 46 fa
    adc dx, bx                                ; 11 da
    add ax, strict word 0ffffh                ; 05 ff ff
    mov word [bp-010h], ax                    ; 89 46 f0
    adc dx, strict byte 0ffffh                ; 83 d2 ff
    mov word [bp-00eh], dx                    ; 89 56 f2
    mov word [bp-006h], bx                    ; 89 5e fa
    mov es, [bp-004h]                         ; 8e 46 fc
    db  066h, 026h, 0c7h, 044h, 014h, 000h, 000h, 000h, 000h
    ; mov dword [es:si+014h], strict dword 000000000h ; 66 26 c7 44 14 00 00 00 00
    mov word [es:si+018h], strict word 00000h ; 26 c7 44 18 00 00
    mov ax, word [bp-010h]                    ; 8b 46 f0
    mov word [es:si], ax                      ; 26 89 04
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    mov word [es:si+002h], ax                 ; 26 89 44 02
    mov ax, word [bp+010h]                    ; 8b 46 10
    mov dx, word [bp+006h]                    ; 8b 56 06
    mov word [es:si+004h], ax                 ; 26 89 44 04
    mov word [es:si+006h], dx                 ; 26 89 54 06
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    mov word [es:si+00ah], ax                 ; 26 89 44 0a
    mov word [es:si+00ch], 00200h             ; 26 c7 44 0c 00 02
    mov word [es:si+00eh], di                 ; 26 89 7c 0e
    mov ax, word [bp-008h]                    ; 8b 46 f8
    mov word [es:si+010h], ax                 ; 26 89 44 10
    mov ax, word [bp-006h]                    ; 8b 46 fa
    mov word [es:si+012h], ax                 ; 26 89 44 12
    mov al, byte [bp-002h]                    ; 8a 46 fe
    mov byte [es:si+008h], al                 ; 26 88 44 08
    movzx ax, byte [bp-002h]                  ; 0f b6 46 fe
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    movzx ax, byte [es:bx+01eh]               ; 26 0f b6 47 1e
    mov bx, ax                                ; 89 c3
    sal bx, 002h                              ; c1 e3 02
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    add ax, ax                                ; 01 c0
    add bx, ax                                ; 01 c3
    push ES                                   ; 06
    push si                                   ; 56
    call word [word bx-00004h]                ; ff 97 fc ff
    mov dx, ax                                ; 89 c2
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor al, al                                ; 30 c0
    mov es, [bp-004h]                         ; 8e 46 fc
    mov bx, word [es:si+014h]                 ; 26 8b 5c 14
    or bx, ax                                 ; 09 c3
    mov word [bp+016h], bx                    ; 89 5e 16
    test dl, dl                               ; 84 d2
    je near 053ebh                            ; 0f 84 13 fe
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 e9 c2
    movzx ax, dl                              ; 0f b6 c2
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005eeh                               ; 68 ee 05
    push 006c9h                               ; 68 c9 06
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 14 c3
    add sp, strict byte 0000ah                ; 83 c4 0a
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 00ch                               ; 80 cc 0c
    jmp near 05758h                           ; e9 51 01
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 ba c2
    push 006eah                               ; 68 ea 06
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 f3 c2
    add sp, strict byte 00004h                ; 83 c4 04
    jmp near 053ebh                           ; e9 cb fd
    movzx ax, byte [bp-002h]                  ; 0f b6 46 fe
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-004h]                         ; 8e 46 fc
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov di, word [es:bx+028h]                 ; 26 8b 7f 28
    mov cx, word [es:bx+026h]                 ; 26 8b 4f 26
    mov ax, word [es:bx+02ah]                 ; 26 8b 47 2a
    mov word [bp-00ah], ax                    ; 89 46 f6
    movzx ax, byte [es:si+0019eh]             ; 26 0f b6 84 9e 01
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov byte [bp+016h], 000h                  ; c6 46 16 00
    mov dx, word [bp+014h]                    ; 8b 56 14
    xor dh, dh                                ; 30 f6
    dec di                                    ; 4f
    mov ax, di                                ; 89 f8
    xor ah, ah                                ; 30 e4
    sal ax, 008h                              ; c1 e0 08
    or dx, ax                                 ; 09 c2
    mov word [bp+014h], dx                    ; 89 56 14
    shr di, 002h                              ; c1 ef 02
    and di, 000c0h                            ; 81 e7 c0 00
    mov ax, word [bp-00ah]                    ; 8b 46 f6
    xor ah, ah                                ; 30 e4
    and AL, strict byte 03fh                  ; 24 3f
    or di, ax                                 ; 09 c7
    mov ax, dx                                ; 89 d0
    xor al, dl                                ; 30 d0
    or ax, di                                 ; 09 f8
    mov word [bp+014h], ax                    ; 89 46 14
    mov dx, word [bp+012h]                    ; 8b 56 12
    xor dh, dh                                ; 30 f6
    mov ax, cx                                ; 89 c8
    sal ax, 008h                              ; c1 e0 08
    sub ax, 00100h                            ; 2d 00 01
    or dx, ax                                 ; 09 c2
    mov word [bp+012h], dx                    ; 89 56 12
    mov ax, dx                                ; 89 d0
    xor al, dl                                ; 30 d0
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    or dx, ax                                 ; 09 c2
    mov word [bp+012h], dx                    ; 89 56 12
    jmp near 053ebh                           ; e9 55 fd
    movzx ax, byte [bp-002h]                  ; 0f b6 46 fe
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    imul ax, ax, strict byte 00006h           ; 6b c0 06
    mov es, [bp-004h]                         ; 8e 46 fc
    add si, ax                                ; 01 c6
    mov dx, word [es:si+001c2h]               ; 26 8b 94 c2 01
    add dx, strict byte 00007h                ; 83 c2 07
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 040h                  ; 3c 40
    jne short 056bbh                          ; 75 03
    jmp near 053ebh                           ; e9 30 fd
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 0aah                               ; 80 cc aa
    jmp near 05758h                           ; e9 92 00
    movzx ax, byte [bp-002h]                  ; 0f b6 46 fe
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-004h]                         ; 8e 46 fc
    add si, ax                                ; 01 c6
    mov di, word [es:si+02eh]                 ; 26 8b 7c 2e
    mov ax, word [es:si+02ch]                 ; 26 8b 44 2c
    mov word [bp-008h], ax                    ; 89 46 f8
    mov ax, word [es:si+030h]                 ; 26 8b 44 30
    mov word [bp-006h], ax                    ; 89 46 fa
    mov ax, di                                ; 89 f8
    xor dx, dx                                ; 31 d2
    mov bx, word [bp-008h]                    ; 8b 5e f8
    xor cx, cx                                ; 31 c9
    call 08e19h                               ; e8 29 37
    mov bx, word [bp-006h]                    ; 8b 5e fa
    xor cx, cx                                ; 31 c9
    call 08e19h                               ; e8 21 37
    mov word [bp-010h], ax                    ; 89 46 f0
    mov word [bp-00eh], dx                    ; 89 56 f2
    mov word [bp+014h], dx                    ; 89 56 14
    mov word [bp+012h], ax                    ; 89 46 12
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 003h                               ; 80 cc 03
    mov word [bp+016h], ax                    ; 89 46 16
    jmp near 053efh                           ; e9 dd fc
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 af c1
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005eeh                               ; 68 ee 05
    push 00704h                               ; 68 04 07
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 de c1
    add sp, strict byte 00008h                ; 83 c4 08
    jmp near 053ebh                           ; e9 b6 fc
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 8c c1
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 005eeh                               ; 68 ee 05
    push 00737h                               ; 68 37 07
    jmp near 05482h                           ; e9 32 fd
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    mov bx, word [bp+016h]                    ; 8b 5e 16
    shr bx, 008h                              ; c1 eb 08
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 a2 be
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    jmp near 053feh                           ; e9 8b fc
    db  02eh, 058h
    ; cs pop ax                                 ; 2e 58
    inc si                                    ; 46
    pop ax                                    ; 58
    inc si                                    ; 46
    pop ax                                    ; 58
    inc si                                    ; 46
    pop ax                                    ; 58
    cmc                                       ; f5
    pop bx                                    ; 5b
    mov word [bx+di+046h], bx                 ; 89 59 46
    pop ax                                    ; 58
    db  08fh, 059h, 0f5h
    ; pop word [bx+di-00bh]                     ; 8f 59 f5
    pop bx                                    ; 5b
    inc sp                                    ; 44
    pop sp                                    ; 5c
    inc sp                                    ; 44
    pop sp                                    ; 5c
    inc sp                                    ; 44
    pop sp                                    ; 5c
    inc sp                                    ; 44
    pop sp                                    ; 5c
    or AL, strict byte 05ch                   ; 0c 5c
    inc sp                                    ; 44
    pop sp                                    ; 5c
    inc sp                                    ; 44
    pop sp                                    ; 5c
_int13_harddisk_ext:                         ; 0xf5793 LB 0x4cc
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    sub sp, strict byte 00028h                ; 83 ec 28
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 7a be
    mov word [bp-014h], ax                    ; 89 46 ec
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 6e be
    mov si, 00122h                            ; be 22 01
    mov word [bp-026h], ax                    ; 89 46 da
    xor bx, bx                                ; 31 db
    mov dx, 0008eh                            ; ba 8e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 4f be
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    xor ah, ah                                ; 30 e4
    cmp ax, 00080h                            ; 3d 80 00
    jc short 057ceh                           ; 72 05
    cmp ax, 00090h                            ; 3d 90 00
    jc short 057ech                           ; 72 1e
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 00765h                               ; 68 65 07
    push 005fdh                               ; 68 fd 05
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 27 c1
    add sp, strict byte 0000ah                ; 83 c4 0a
    jmp near 05c22h                           ; e9 36 04
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    xor ah, ah                                ; 30 e4
    mov es, [bp-026h]                         ; 8e 46 da
    mov bx, si                                ; 89 f3
    add bx, ax                                ; 01 c3
    mov cl, byte [es:bx+0011fh]               ; 26 8a 8f 1f 01
    cmp cl, 010h                              ; 80 f9 10
    jc short 05812h                           ; 72 10
    push ax                                   ; 50
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 00765h                               ; 68 65 07
    push 00628h                               ; 68 28 06
    jmp short 057e1h                          ; eb cf
    mov bx, word [bp+016h]                    ; 8b 5e 16
    shr bx, 008h                              ; c1 eb 08
    sub bx, strict byte 00041h                ; 83 eb 41
    cmp bx, strict byte 0000fh                ; 83 fb 0f
    jnbe near 05c44h                          ; 0f 87 22 04
    add bx, bx                                ; 01 db
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    jmp word [cs:bx+05773h]                   ; 2e ff a7 73 57
    mov word [bp+010h], 0aa55h                ; c7 46 10 55 aa
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 030h                               ; 80 cc 30
    mov word [bp+016h], ax                    ; 89 46 16
    mov word [bp+014h], strict word 00007h    ; c7 46 14 07 00
    jmp near 05bf9h                           ; e9 b3 03
    mov bx, word [bp+00ah]                    ; 8b 5e 0a
    mov es, [bp+004h]                         ; 8e 46 04
    mov di, bx                                ; 89 df
    mov [bp-010h], es                         ; 8c 46 f0
    mov ax, word [es:bx+002h]                 ; 26 8b 47 02
    mov word [bp-012h], ax                    ; 89 46 ee
    mov ax, word [es:bx+006h]                 ; 26 8b 47 06
    mov word [bp-018h], ax                    ; 89 46 e8
    mov ax, word [es:bx+004h]                 ; 26 8b 47 04
    mov word [bp-016h], ax                    ; 89 46 ea
    mov ax, word [es:bx+00ch]                 ; 26 8b 47 0c
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov ax, word [es:bx+00eh]                 ; 26 8b 47 0e
    or ax, word [bp-00ah]                     ; 0b 46 f6
    je short 05887h                           ; 74 11
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 00765h                               ; 68 65 07
    push 00778h                               ; 68 78 07
    push strict byte 00007h                   ; 6a 07
    jmp short 058d1h                          ; eb 4a
    mov es, [bp-010h]                         ; 8e 46 f0
    mov ax, word [es:di+008h]                 ; 26 8b 45 08
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov ax, word [es:di+00ah]                 ; 26 8b 45 0a
    movzx dx, cl                              ; 0f b6 d1
    imul dx, dx, strict byte 00018h           ; 6b d2 18
    mov es, [bp-026h]                         ; 8e 46 da
    mov bx, si                                ; 89 f3
    add bx, dx                                ; 01 d3
    mov ch, byte [es:bx+01eh]                 ; 26 8a 6f 1e
    cmp ax, word [es:bx+034h]                 ; 26 3b 47 34
    jnbe short 058b7h                         ; 77 0b
    jne short 058dah                          ; 75 2c
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    cmp dx, word [es:bx+032h]                 ; 26 3b 57 32
    jc short 058dah                           ; 72 23
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 0a c0
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 00765h                               ; 68 65 07
    push 007a1h                               ; 68 a1 07
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 39 c0
    add sp, strict byte 00008h                ; 83 c4 08
    jmp near 05c22h                           ; e9 48 03
    mov dx, word [bp+016h]                    ; 8b 56 16
    shr dx, 008h                              ; c1 ea 08
    mov word [bp-00ch], dx                    ; 89 56 f4
    cmp dx, strict byte 00044h                ; 83 fa 44
    je near 05bf5h                            ; 0f 84 0b 03
    cmp dx, strict byte 00047h                ; 83 fa 47
    je near 05bf5h                            ; 0f 84 04 03
    mov es, [bp-026h]                         ; 8e 46 da
    db  066h, 026h, 0c7h, 044h, 014h, 000h, 000h, 000h, 000h
    ; mov dword [es:si+014h], strict dword 000000000h ; 66 26 c7 44 14 00 00 00 00
    mov word [es:si+018h], strict word 00000h ; 26 c7 44 18 00 00
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    mov word [es:si], dx                      ; 26 89 14
    mov word [es:si+002h], ax                 ; 26 89 44 02
    mov ax, word [bp-016h]                    ; 8b 46 ea
    mov word [es:si+004h], ax                 ; 26 89 44 04
    mov ax, word [bp-018h]                    ; 8b 46 e8
    mov word [es:si+006h], ax                 ; 26 89 44 06
    mov ax, word [bp-012h]                    ; 8b 46 ee
    mov word [es:si+00ah], ax                 ; 26 89 44 0a
    mov word [es:si+00ch], 00200h             ; 26 c7 44 0c 00 02
    mov word [es:si+012h], strict word 00000h ; 26 c7 44 12 00 00
    mov byte [es:si+008h], cl                 ; 26 88 4c 08
    mov bx, word [bp-00ch]                    ; 8b 5e f4
    add bx, bx                                ; 01 db
    movzx ax, ch                              ; 0f b6 c5
    sal ax, 002h                              ; c1 e0 02
    add bx, ax                                ; 01 c3
    push ES                                   ; 06
    push si                                   ; 56
    call word [bx-00084h]                     ; ff 97 7c ff
    mov dx, ax                                ; 89 c2
    mov es, [bp-026h]                         ; 8e 46 da
    mov ax, word [es:si+014h]                 ; 26 8b 44 14
    mov word [bp-012h], ax                    ; 89 46 ee
    mov es, [bp-010h]                         ; 8e 46 f0
    mov word [es:di+002h], ax                 ; 26 89 45 02
    test dl, dl                               ; 84 d2
    je near 05bf5h                            ; 0f 84 97 02
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 63 bf
    movzx ax, dl                              ; 0f b6 c2
    push ax                                   ; 50
    push word [bp-00ch]                       ; ff 76 f4
    push 00765h                               ; 68 65 07
    push 006c9h                               ; 68 c9 06
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 92 bf
    add sp, strict byte 0000ah                ; 83 c4 0a
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 00ch                               ; 80 cc 0c
    jmp near 05c2ah                           ; e9 a1 02
    or ah, 0b2h                               ; 80 cc b2
    jmp near 05c2ah                           ; e9 9b 02
    mov bx, word [bp+00ah]                    ; 8b 5e 0a
    mov ax, word [bp+004h]                    ; 8b 46 04
    mov word [bp-006h], ax                    ; 89 46 fa
    mov word [bp-004h], ax                    ; 89 46 fc
    mov es, ax                                ; 8e c0
    mov di, bx                                ; 89 df
    mov ax, word [es:di]                      ; 26 8b 05
    mov word [bp-00eh], ax                    ; 89 46 f2
    cmp ax, strict word 0001ah                ; 3d 1a 00
    jc near 05c22h                            ; 0f 82 76 02
    jc near 05a33h                            ; 0f 82 83 00
    movzx ax, cl                              ; 0f b6 c1
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    mov es, [bp-026h]                         ; 8e 46 da
    mov di, si                                ; 89 f7
    add di, ax                                ; 01 c7
    mov ax, word [es:di+02eh]                 ; 26 8b 45 2e
    mov word [bp-028h], ax                    ; 89 46 d8
    mov ax, word [es:di+02ch]                 ; 26 8b 45 2c
    mov word [bp-01eh], ax                    ; 89 46 e2
    mov ax, word [es:di+030h]                 ; 26 8b 45 30
    mov word [bp-024h], ax                    ; 89 46 dc
    mov ax, word [es:di+032h]                 ; 26 8b 45 32
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov ax, word [es:di+034h]                 ; 26 8b 45 34
    mov dx, word [es:di+024h]                 ; 26 8b 55 24
    mov word [bp-022h], dx                    ; 89 56 de
    mov es, [bp-006h]                         ; 8e 46 fa
    mov di, bx                                ; 89 df
    db  066h, 026h, 0c7h, 005h, 01ah, 000h, 002h, 000h
    ; mov dword [es:di], strict dword 00002001ah ; 66 26 c7 05 1a 00 02 00
    mov dx, word [bp-028h]                    ; 8b 56 d8
    mov word [es:di+004h], dx                 ; 26 89 55 04
    mov word [es:di+006h], strict word 00000h ; 26 c7 45 06 00 00
    mov dx, word [bp-01eh]                    ; 8b 56 e2
    mov word [es:di+008h], dx                 ; 26 89 55 08
    mov word [es:di+00ah], strict word 00000h ; 26 c7 45 0a 00 00
    mov dx, word [bp-024h]                    ; 8b 56 dc
    mov word [es:di+00ch], dx                 ; 26 89 55 0c
    mov word [es:di+00eh], strict word 00000h ; 26 c7 45 0e 00 00
    mov dx, word [bp-022h]                    ; 8b 56 de
    mov word [es:di+018h], dx                 ; 26 89 55 18
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    mov word [es:di+010h], dx                 ; 26 89 55 10
    mov word [es:di+012h], ax                 ; 26 89 45 12
    db  066h, 026h, 0c7h, 045h, 014h, 000h, 000h, 000h, 000h
    ; mov dword [es:di+014h], strict dword 000000000h ; 66 26 c7 45 14 00 00 00 00
    cmp word [bp-00eh], strict byte 0001eh    ; 83 7e f2 1e
    jc near 05b3fh                            ; 0f 82 04 01
    mov es, [bp-004h]                         ; 8e 46 fc
    mov word [es:bx], strict word 0001eh      ; 26 c7 07 1e 00
    mov ax, word [bp-014h]                    ; 8b 46 ec
    mov word [es:bx+01ch], ax                 ; 26 89 47 1c
    mov word [es:bx+01ah], 00312h             ; 26 c7 47 1a 12 03
    movzx ax, cl                              ; 0f b6 c1
    mov word [bp-020h], ax                    ; 89 46 e0
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    movzx di, al                              ; 0f b6 f8
    imul di, di, strict byte 00006h           ; 6b ff 06
    mov es, [bp-026h]                         ; 8e 46 da
    add di, si                                ; 01 f7
    mov ax, word [es:di+001c2h]               ; 26 8b 85 c2 01
    mov word [bp-01ah], ax                    ; 89 46 e6
    mov ax, word [es:di+001c4h]               ; 26 8b 85 c4 01
    mov word [bp-01ch], ax                    ; 89 46 e4
    mov ch, byte [es:di+001c1h]               ; 26 8a ad c1 01
    imul di, word [bp-020h], strict byte 00018h ; 6b 7e e0 18
    add di, si                                ; 01 f7
    mov ah, byte [es:di+022h]                 ; 26 8a 65 22
    mov al, byte [es:di+023h]                 ; 26 8a 45 23
    test al, al                               ; 84 c0
    jne short 05a91h                          ; 75 04
    xor dx, dx                                ; 31 d2
    jmp short 05a94h                          ; eb 03
    mov dx, strict word 00008h                ; ba 08 00
    or dl, 010h                               ; 80 ca 10
    mov word [bp-008h], dx                    ; 89 56 f8
    cmp ah, 001h                              ; 80 fc 01
    db  00fh, 094h, 0c4h
    ; sete ah                                   ; 0f 94 c4
    movzx dx, ah                              ; 0f b6 d4
    or word [bp-008h], dx                     ; 09 56 f8
    cmp AL, strict byte 001h                  ; 3c 01
    db  00fh, 094h, 0c4h
    ; sete ah                                   ; 0f 94 c4
    movzx dx, ah                              ; 0f b6 d4
    or word [bp-008h], dx                     ; 09 56 f8
    cmp AL, strict byte 003h                  ; 3c 03
    jne short 05abah                          ; 75 05
    mov ax, strict word 00003h                ; b8 03 00
    jmp short 05abch                          ; eb 02
    xor ax, ax                                ; 31 c0
    or word [bp-008h], ax                     ; 09 46 f8
    mov ax, word [bp-01ah]                    ; 8b 46 e6
    mov es, [bp-026h]                         ; 8e 46 da
    mov word [es:si+001f0h], ax               ; 26 89 84 f0 01
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    mov word [es:si+001f2h], ax               ; 26 89 84 f2 01
    movzx ax, cl                              ; 0f b6 c1
    cwd                                       ; 99
    mov di, strict word 00002h                ; bf 02 00
    idiv di                                   ; f7 ff
    or dl, 00eh                               ; 80 ca 0e
    sal dx, 004h                              ; c1 e2 04
    mov byte [es:si+001f4h], dl               ; 26 88 94 f4 01
    mov byte [es:si+001f5h], 0cbh             ; 26 c6 84 f5 01 cb
    mov byte [es:si+001f6h], ch               ; 26 88 ac f6 01
    mov word [es:si+001f7h], strict word 00001h ; 26 c7 84 f7 01 01 00
    mov byte [es:si+001f9h], 000h             ; 26 c6 84 f9 01 00
    mov ax, word [bp-008h]                    ; 8b 46 f8
    mov word [es:si+001fah], ax               ; 26 89 84 fa 01
    mov word [es:si+001fch], strict word 00000h ; 26 c7 84 fc 01 00 00
    mov byte [es:si+001feh], 011h             ; 26 c6 84 fe 01 11
    xor ch, ch                                ; 30 ed
    mov byte [bp-002h], ch                    ; 88 6e fe
    jmp short 05b20h                          ; eb 06
    cmp byte [bp-002h], 00fh                  ; 80 7e fe 0f
    jnc short 05b35h                          ; 73 15
    movzx dx, byte [bp-002h]                  ; 0f b6 56 fe
    add dx, 00312h                            ; 81 c2 12 03
    mov ax, word [bp-014h]                    ; 8b 46 ec
    call 01600h                               ; e8 d2 ba
    add ch, al                                ; 00 c5
    inc byte [bp-002h]                        ; fe 46 fe
    jmp short 05b1ah                          ; eb e5
    neg ch                                    ; f6 dd
    mov es, [bp-026h]                         ; 8e 46 da
    mov byte [es:si+001ffh], ch               ; 26 88 ac ff 01
    cmp word [bp-00eh], strict byte 00042h    ; 83 7e f2 42
    jc near 05bf5h                            ; 0f 82 ae 00
    movzx ax, cl                              ; 0f b6 c1
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    xor ah, ah                                ; 30 e4
    imul ax, ax, strict byte 00006h           ; 6b c0 06
    mov es, [bp-026h]                         ; 8e 46 da
    add si, ax                                ; 01 c6
    mov al, byte [es:si+001c0h]               ; 26 8a 84 c0 01
    mov dx, word [es:si+001c2h]               ; 26 8b 94 c2 01
    mov es, [bp-004h]                         ; 8e 46 fc
    mov word [es:bx], strict word 00042h      ; 26 c7 07 42 00
    db  066h, 026h, 0c7h, 047h, 01eh, 0ddh, 0beh, 024h, 000h
    ; mov dword [es:bx+01eh], strict dword 00024beddh ; 66 26 c7 47 1e dd be 24 00
    mov word [es:bx+022h], strict word 00000h ; 26 c7 47 22 00 00
    test al, al                               ; 84 c0
    jne short 05b87h                          ; 75 09
    db  066h, 026h, 0c7h, 047h, 024h, 049h, 053h, 041h, 020h
    ; mov dword [es:bx+024h], strict dword 020415349h ; 66 26 c7 47 24 49 53 41 20
    mov es, [bp-004h]                         ; 8e 46 fc
    db  066h, 026h, 0c7h, 047h, 028h, 041h, 054h, 041h, 020h
    ; mov dword [es:bx+028h], strict dword 020415441h ; 66 26 c7 47 28 41 54 41 20
    db  066h, 026h, 0c7h, 047h, 02ch, 020h, 020h, 020h, 020h
    ; mov dword [es:bx+02ch], strict dword 020202020h ; 66 26 c7 47 2c 20 20 20 20
    test al, al                               ; 84 c0
    jne short 05bb3h                          ; 75 13
    mov word [es:bx+030h], dx                 ; 26 89 57 30
    db  066h, 026h, 0c7h, 047h, 032h, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+032h], strict dword 000000000h ; 66 26 c7 47 32 00 00 00 00
    mov word [es:bx+036h], strict word 00000h ; 26 c7 47 36 00 00
    mov al, cl                                ; 88 c8
    and AL, strict byte 001h                  ; 24 01
    xor ah, ah                                ; 30 e4
    mov es, [bp-004h]                         ; 8e 46 fc
    mov word [es:bx+038h], ax                 ; 26 89 47 38
    db  066h, 026h, 0c7h, 047h, 03ah, 000h, 000h, 000h, 000h
    ; mov dword [es:bx+03ah], strict dword 000000000h ; 66 26 c7 47 3a 00 00 00 00
    mov word [es:bx+03eh], strict word 00000h ; 26 c7 47 3e 00 00
    xor cl, cl                                ; 30 c9
    mov CH, strict byte 01eh                  ; b5 1e
    jmp short 05bdah                          ; eb 05
    cmp ch, 040h                              ; 80 fd 40
    jnc short 05bech                          ; 73 12
    movzx dx, ch                              ; 0f b6 d5
    add dx, word [bp+00ah]                    ; 03 56 0a
    mov ax, word [bp+004h]                    ; 8b 46 04
    call 01600h                               ; e8 1a ba
    add cl, al                                ; 00 c1
    db  0feh, 0c5h
    ; inc ch                                    ; fe c5
    jmp short 05bd5h                          ; eb e9
    neg cl                                    ; f6 d9
    mov es, [bp-004h]                         ; 8e 46 fc
    mov byte [es:bx+041h], cl                 ; 26 88 4f 41
    mov byte [bp+017h], 000h                  ; c6 46 17 00
    xor bx, bx                                ; 31 db
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 0a ba
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
    cmp ax, strict word 00006h                ; 3d 06 00
    je short 05bf5h                           ; 74 e4
    cmp ax, strict word 00001h                ; 3d 01 00
    jc short 05c22h                           ; 72 0c
    jbe short 05bf5h                          ; 76 dd
    cmp ax, strict word 00003h                ; 3d 03 00
    jc short 05c22h                           ; 72 05
    cmp ax, strict word 00004h                ; 3d 04 00
    jbe short 05bf5h                          ; 76 d3
    mov ax, word [bp+016h]                    ; 8b 46 16
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov word [bp+016h], ax                    ; 89 46 16
    mov bx, word [bp+016h]                    ; 8b 5e 16
    shr bx, 008h                              ; c1 eb 08
    xor bh, bh                                ; 30 ff
    mov dx, strict word 00074h                ; ba 74 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 d0 b9
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    jmp short 05c08h                          ; eb c4
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 7d bc
    mov ax, word [bp+016h]                    ; 8b 46 16
    shr ax, 008h                              ; c1 e8 08
    push ax                                   ; 50
    push 00765h                               ; 68 65 07
    push 00737h                               ; 68 37 07
    jmp near 058cfh                           ; e9 70 fc
_int14_function:                             ; 0xf5c5f LB 0x155
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    sti                                       ; fb
    mov dx, word [bp+00eh]                    ; 8b 56 0e
    add dx, dx                                ; 01 d2
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 ad b9
    mov si, ax                                ; 89 c6
    mov bx, ax                                ; 89 c3
    mov dx, word [bp+00eh]                    ; 8b 56 0e
    add dx, strict byte 0007ch                ; 83 c2 7c
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 81 b9
    mov cl, al                                ; 88 c1
    cmp word [bp+00eh], strict byte 00004h    ; 83 7e 0e 04
    jnc near 05daah                           ; 0f 83 21 01
    test si, si                               ; 85 f6
    jbe near 05daah                           ; 0f 86 1b 01
    mov al, byte [bp+013h]                    ; 8a 46 13
    cmp AL, strict byte 001h                  ; 3c 01
    jc short 05ca7h                           ; 72 11
    jbe short 05cfbh                          ; 76 63
    cmp AL, strict byte 003h                  ; 3c 03
    je near 05d93h                            ; 0f 84 f5 00
    cmp AL, strict byte 002h                  ; 3c 02
    je near 05d49h                            ; 0f 84 a5 00
    jmp near 05da4h                           ; e9 fd 00
    test al, al                               ; 84 c0
    jne near 05da4h                           ; 0f 85 f7 00
    lea dx, [bx+003h]                         ; 8d 57 03
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    or AL, strict byte 080h                   ; 0c 80
    out DX, AL                                ; ee
    mov al, byte [bp+012h]                    ; 8a 46 12
    and AL, strict byte 0e0h                  ; 24 e0
    movzx cx, al                              ; 0f b6 c8
    sar cx, 005h                              ; c1 f9 05
    mov ax, 00600h                            ; b8 00 06
    sar ax, CL                                ; d3 f8
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    shr ax, 008h                              ; c1 e8 08
    lea dx, [bx+001h]                         ; 8d 57 01
    out DX, AL                                ; ee
    mov al, byte [bp+012h]                    ; 8a 46 12
    and AL, strict byte 01fh                  ; 24 1f
    lea dx, [bx+003h]                         ; 8d 57 03
    out DX, AL                                ; ee
    lea dx, [bx+005h]                         ; 8d 57 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+013h], al                    ; 88 46 13
    lea dx, [bx+006h]                         ; 8d 57 06
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+012h], al                    ; 88 46 12
    jmp near 05d85h                           ; e9 97 00
    mov AL, strict byte 017h                  ; b0 17
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    lea dx, [bx+001h]                         ; 8d 57 01
    mov AL, strict byte 004h                  ; b0 04
    out DX, AL                                ; ee
    jmp short 05cd0h                          ; eb d5
    mov dx, strict word 0006ch                ; ba 6c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 18 b9
    mov si, ax                                ; 89 c6
    lea dx, [bx+005h]                         ; 8d 57 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and ax, strict word 00060h                ; 25 60 00
    cmp ax, strict word 00060h                ; 3d 60 00
    je short 05d2bh                           ; 74 17
    test cl, cl                               ; 84 c9
    je short 05d2bh                           ; 74 13
    mov dx, strict word 0006ch                ; ba 6c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 fb b8
    cmp ax, si                                ; 39 f0
    je short 05d06h                           ; 74 e1
    mov si, ax                                ; 89 c6
    db  0feh, 0c9h
    ; dec cl                                    ; fe c9
    jmp short 05d06h                          ; eb db
    test cl, cl                               ; 84 c9
    je short 05d35h                           ; 74 06
    mov al, byte [bp+012h]                    ; 8a 46 12
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    lea dx, [bx+005h]                         ; 8d 57 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+013h], al                    ; 88 46 13
    test cl, cl                               ; 84 c9
    jne short 05d85h                          ; 75 43
    or AL, strict byte 080h                   ; 0c 80
    mov byte [bp+013h], al                    ; 88 46 13
    jmp short 05d85h                          ; eb 3c
    mov dx, strict word 0006ch                ; ba 6c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 ca b8
    mov si, ax                                ; 89 c6
    lea dx, [bx+005h]                         ; 8d 57 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 05d75h                          ; 75 17
    test cl, cl                               ; 84 c9
    je short 05d75h                           ; 74 13
    mov dx, strict word 0006ch                ; ba 6c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 b1 b8
    cmp ax, si                                ; 39 f0
    je short 05d54h                           ; 74 e5
    mov si, ax                                ; 89 c6
    db  0feh, 0c9h
    ; dec cl                                    ; fe c9
    jmp short 05d54h                          ; eb df
    test cl, cl                               ; 84 c9
    je short 05d8bh                           ; 74 12
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    mov dx, bx                                ; 89 da
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+012h], al                    ; 88 46 12
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    jmp short 05daeh                          ; eb 23
    lea dx, [bx+005h]                         ; 8d 57 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    jmp short 05d44h                          ; eb b1
    lea dx, [si+005h]                         ; 8d 54 05
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp+013h], al                    ; 88 46 13
    lea dx, [si+006h]                         ; 8d 54 06
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    jmp short 05d82h                          ; eb de
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    jmp short 05daeh                          ; eb 04
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
set_enable_a20_:                             ; 0xf5db4 LB 0x2c
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    mov bx, ax                                ; 89 c3
    mov dx, 00092h                            ; ba 92 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov cl, al                                ; 88 c1
    test bx, bx                               ; 85 db
    je short 05dcdh                           ; 74 05
    or AL, strict byte 002h                   ; 0c 02
    out DX, AL                                ; ee
    jmp short 05dd0h                          ; eb 03
    and AL, strict byte 0fdh                  ; 24 fd
    out DX, AL                                ; ee
    test cl, 002h                             ; f6 c1 02
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
set_e820_range_:                             ; 0xf5de0 LB 0x8c
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov es, ax                                ; 8e c0
    mov si, dx                                ; 89 d6
    mov word [es:si], bx                      ; 26 89 1c
    mov word [es:si+002h], cx                 ; 26 89 4c 02
    movzx ax, byte [bp+00ah]                  ; 0f b6 46 0a
    mov word [es:si+004h], ax                 ; 26 89 44 04
    mov word [es:si+006h], strict word 00000h ; 26 c7 44 06 00 00
    sub word [bp+006h], bx                    ; 29 5e 06
    sbb word [bp+008h], cx                    ; 19 4e 08
    mov al, byte [bp+00ah]                    ; 8a 46 0a
    sub byte [bp+00ch], al                    ; 28 46 0c
    mov ax, word [bp+006h]                    ; 8b 46 06
    mov word [es:si+008h], ax                 ; 26 89 44 08
    mov ax, word [bp+008h]                    ; 8b 46 08
    mov word [es:si+00ah], ax                 ; 26 89 44 0a
    movzx ax, byte [bp+00ch]                  ; 0f b6 46 0c
    mov word [es:si+00ch], ax                 ; 26 89 44 0c
    mov word [es:si+00eh], strict word 00000h ; 26 c7 44 0e 00 00
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    mov word [es:si+010h], ax                 ; 26 89 44 10
    mov word [es:si+012h], strict word 00000h ; 26 c7 44 12 00 00
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn 0000ah                               ; c2 0a 00
    in AL, DX                                 ; ec
    jmp near 02013h                           ; e9 d8 c1
    sar byte [bx-06f6fh], 089h                ; c0 bf 91 90 89
    mov byte [bx+05283h], al                  ; 88 87 83 52
    dec di                                    ; 4f
    inc cx                                    ; 41
    and AL, strict byte 000h                  ; 24 00
    or sp, word [bp+di-055h]                  ; 0b 63 ab
    pop si                                    ; 5e
    mov si, 0535eh                            ; be 5e 53
    pop di                                    ; 5f
    pop cx                                    ; 59
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop di                                    ; 5f
    arpl word [bx+005h], bx                   ; 63 5f 05
    pushaw                                    ; 60
    mov byte [0c861h], AL                     ; a2 61 c8
    popaw                                     ; 61
    dec sp                                    ; 4c
    pop di                                    ; 5f
    dec sp                                    ; 4c
    pop di                                    ; 5f
    xchg bp, ax                               ; 95
    bound di, [di-02f9eh]                     ; 62 bd 62 d0
    db  062h
    fbld [bp+si+053h]                         ; df 62 53
    pop di                                    ; 5f
    out strict byte 062h, AL                  ; e6 62
_int15_function:                             ; 0xf5e6c LB 0x4cd
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov ax, word [bp+012h]                    ; 8b 46 12
    shr ax, 008h                              ; c1 e8 08
    cmp ax, 000ech                            ; 3d ec 00
    jnbe near 0630bh                          ; 0f 87 8b 04
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 00012h                ; b9 12 00
    mov di, 05e37h                            ; bf 37 5e
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov si, word [cs:di+05e48h]               ; 2e 8b b5 48 5e
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    mov cx, word [bp+018h]                    ; 8b 4e 18
    and cl, 0feh                              ; 80 e1 fe
    mov bx, word [bp+018h]                    ; 8b 5e 18
    or bl, 001h                               ; 80 cb 01
    mov dx, ax                                ; 89 c2
    or dh, 086h                               ; 80 ce 86
    jmp si                                    ; ff e6
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    cmp ax, 000c0h                            ; 3d c0 00
    jne near 0630bh                           ; 0f 85 54 04
    or byte [bp+018h], 001h                   ; 80 4e 18 01
    jmp near 062b4h                           ; e9 f6 03
    mov dx, ax                                ; 89 c2
    cmp ax, strict word 00001h                ; 3d 01 00
    jc short 05ed3h                           ; 72 0e
    jbe short 05ee7h                          ; 76 20
    cmp ax, strict word 00003h                ; 3d 03 00
    je short 05f14h                           ; 74 48
    cmp ax, strict word 00002h                ; 3d 02 00
    je short 05ef7h                           ; 74 26
    jmp short 05f21h                          ; eb 4e
    test ax, ax                               ; 85 c0
    jne short 05f21h                          ; 75 4a
    xor ax, ax                                ; 31 c0
    call 05db4h                               ; e8 d8 fe
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    jmp near 05f4ch                           ; e9 65 00
    mov ax, strict word 00001h                ; b8 01 00
    call 05db4h                               ; e8 c7 fe
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    mov byte [bp+013h], dh                    ; 88 76 13
    jmp near 05f4ch                           ; e9 55 00
    mov dx, 00092h                            ; ba 92 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    shr ax, 1                                 ; d1 e8
    and ax, strict word 00001h                ; 25 01 00
    mov dx, word [bp+012h]                    ; 8b 56 12
    mov dl, al                                ; 88 c2
    mov word [bp+012h], dx                    ; 89 56 12
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    mov byte [bp+013h], ah                    ; 88 66 13
    jmp near 05f4ch                           ; e9 38 00
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    mov byte [bp+013h], ah                    ; 88 66 13
    mov word [bp+00ch], ax                    ; 89 46 0c
    jmp near 05f4ch                           ; e9 2b 00
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 a0 b9
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    push 007c6h                               ; 68 c6 07
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 d3 b9
    add sp, strict byte 00006h                ; 83 c4 06
    or byte [bp+018h], 001h                   ; 80 4e 18 01
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    or ah, 086h                               ; 80 cc 86
    mov word [bp+012h], ax                    ; 89 46 12
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    mov word [bp+018h], bx                    ; 89 5e 18
    jmp near 05fffh                           ; e9 a6 00
    mov word [bp+018h], bx                    ; 89 5e 18
    jmp short 05f4ch                          ; eb ee
    mov word [bp+018h], cx                    ; 89 4e 18
    jmp short 05f49h                          ; eb e6
    test byte [bp+012h], 0ffh                 ; f6 46 12 ff
    je short 05fd5h                           ; 74 6c
    mov dx, 000a0h                            ; ba a0 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 8e b6
    test AL, strict byte 001h                 ; a8 01
    jne near 062abh                           ; 0f 85 33 03
    mov bx, strict word 00001h                ; bb 01 00
    mov dx, 000a0h                            ; ba a0 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 8a b6
    mov bx, word [bp+014h]                    ; 8b 5e 14
    mov dx, 00098h                            ; ba 98 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0162ah                               ; e8 9a b6
    mov bx, word [bp+00ch]                    ; 8b 5e 0c
    mov dx, 0009ah                            ; ba 9a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0162ah                               ; e8 8e b6
    mov bx, word [bp+00eh]                    ; 8b 5e 0e
    mov dx, 0009ch                            ; ba 9c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0162ah                               ; e8 82 b6
    mov bx, word [bp+010h]                    ; 8b 5e 10
    mov dx, 0009eh                            ; ba 9e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0162ah                               ; e8 76 b6
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    mov dx, 000a1h                            ; ba a1 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0feh                  ; 24 fe
    out DX, AL                                ; ee
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 95 b6
    or AL, strict byte 040h                   ; 0c 40
    movzx dx, al                              ; 0f b6 d0
    mov ax, strict word 0000bh                ; b8 0b 00
    call 01670h                               ; e8 9e b6
    jmp near 05f4ch                           ; e9 77 ff
    cmp ax, strict word 00001h                ; 3d 01 00
    jne short 05ff3h                          ; 75 19
    xor bx, bx                                ; 31 db
    mov dx, 000a0h                            ; ba a0 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 29 b6
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 6d b6
    and AL, strict byte 0bfh                  ; 24 bf
    jmp short 05fc9h                          ; eb d6
    mov word [bp+018h], bx                    ; 89 5e 18
    mov ax, dx                                ; 89 d0
    xor ah, dh                                ; 30 f4
    xor dl, dl                                ; 30 d2
    dec ax                                    ; 48
    or dx, ax                                 ; 09 c2
    mov word [bp+012h], dx                    ; 89 56 12
    jmp near 05f4ch                           ; e9 47 ff
    cli                                       ; fa
    mov ax, strict word 00001h                ; b8 01 00
    call 05db4h                               ; e8 a8 fd
    mov di, ax                                ; 89 c7
    mov ax, word [bp+014h]                    ; 8b 46 14
    sal ax, 004h                              ; c1 e0 04
    mov cx, word [bp+006h]                    ; 8b 4e 06
    add cx, ax                                ; 01 c1
    mov dx, word [bp+014h]                    ; 8b 56 14
    shr dx, 00ch                              ; c1 ea 0c
    mov byte [bp-006h], dl                    ; 88 56 fa
    cmp cx, ax                                ; 39 c1
    jnc short 0602bh                          ; 73 05
    db  0feh, 0c2h
    ; inc dl                                    ; fe c2
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00008h                ; 83 c2 08
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, strict word 0002fh                ; bb 2f 00
    call 0162ah                               ; e8 f0 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0000ah                ; 83 c2 0a
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, cx                                ; 89 cb
    call 0162ah                               ; e8 e2 b5
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0000ch                ; 83 c2 0c
    mov ax, word [bp+014h]                    ; 8b 46 14
    call 0160eh                               ; e8 b6 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0000dh                ; 83 c2 0d
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, 00093h                            ; bb 93 00
    call 0160eh                               ; e8 a7 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0000eh                ; 83 c2 0e
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor bx, bx                                ; 31 db
    call 0162ah                               ; e8 b5 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00020h                ; 83 c2 20
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, strict word 0ffffh                ; bb ff ff
    call 0162ah                               ; e8 a6 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00022h                ; 83 c2 22
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor bx, bx                                ; 31 db
    call 0162ah                               ; e8 98 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00024h                ; 83 c2 24
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, strict word 0000fh                ; bb 0f 00
    call 0160eh                               ; e8 6d b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00025h                ; 83 c2 25
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, 0009bh                            ; bb 9b 00
    call 0160eh                               ; e8 5e b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00026h                ; 83 c2 26
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor bx, bx                                ; 31 db
    call 0162ah                               ; e8 6c b5
    mov ax, ss                                ; 8c d0
    mov cx, ax                                ; 89 c1
    sal cx, 004h                              ; c1 e1 04
    shr ax, 00ch                              ; c1 e8 0c
    mov word [bp-008h], ax                    ; 89 46 f8
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00028h                ; 83 c2 28
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, strict word 0ffffh                ; bb ff ff
    call 0162ah                               ; e8 50 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0002ah                ; 83 c2 2a
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, cx                                ; 89 cb
    call 0162ah                               ; e8 42 b5
    movzx bx, byte [bp-008h]                  ; 0f b6 5e f8
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0002ch                ; 83 c2 2c
    mov ax, word [bp+014h]                    ; 8b 46 14
    call 0160eh                               ; e8 16 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0002dh                ; 83 c2 2d
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, 00093h                            ; bb 93 00
    call 0160eh                               ; e8 07 b5
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0002eh                ; 83 c2 2e
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor bx, bx                                ; 31 db
    call 0162ah                               ; e8 15 b5
    mov si, word [bp+006h]                    ; 8b 76 06
    mov es, [bp+014h]                         ; 8e 46 14
    mov cx, word [bp+010h]                    ; 8b 4e 10
    push DS                                   ; 1e
    push eax                                  ; 66 50
    db  066h, 033h, 0c0h
    ; xor eax, eax                              ; 66 33 c0
    mov ds, ax                                ; 8e d8
    mov word [00467h], sp                     ; 89 26 67 04
    mov [00469h], ss                          ; 8c 16 69 04
    call 06131h                               ; e8 00 00
    pop di                                    ; 5f
    add di, strict byte 0001bh                ; 83 c7 1b
    push strict byte 00020h                   ; 6a 20
    push di                                   ; 57
    lgdt [es:si+008h]                         ; 26 0f 01 54 08
    lidt [cs:0efe1h]                          ; 2e 0f 01 1e e1 ef
    mov eax, cr0                              ; 0f 20 c0
    or AL, strict byte 001h                   ; 0c 01
    mov cr0, eax                              ; 0f 22 c0
    retf                                      ; cb
    mov ax, strict word 00028h                ; b8 28 00
    mov ss, ax                                ; 8e d0
    mov ax, strict word 00010h                ; b8 10 00
    mov ds, ax                                ; 8e d8
    mov ax, strict word 00018h                ; b8 18 00
    mov es, ax                                ; 8e c0
    db  033h, 0f6h
    ; xor si, si                                ; 33 f6
    db  033h, 0ffh
    ; xor di, di                                ; 33 ff
    cld                                       ; fc
    rep movsw                                 ; f3 a5
    call 06165h                               ; e8 00 00
    pop ax                                    ; 58
    push 0f000h                               ; 68 00 f0
    add ax, strict byte 00018h                ; 83 c0 18
    push ax                                   ; 50
    mov ax, strict word 00028h                ; b8 28 00
    mov ds, ax                                ; 8e d8
    mov es, ax                                ; 8e c0
    mov eax, cr0                              ; 0f 20 c0
    and AL, strict byte 0feh                  ; 24 fe
    mov cr0, eax                              ; 0f 22 c0
    retf                                      ; cb
    lidt [cs:0efe7h]                          ; 2e 0f 01 1e e7 ef
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov es, ax                                ; 8e c0
    lss sp, [00467h]                          ; 0f b2 26 67 04
    pop eax                                   ; 66 58
    pop DS                                    ; 1f
    mov ax, di                                ; 89 f8
    call 05db4h                               ; e8 1e fc
    sti                                       ; fb
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    jmp near 05f4ch                           ; e9 aa fd
    mov ax, strict word 00031h                ; b8 31 00
    call 0165ch                               ; e8 b4 b4
    xor ah, ah                                ; 30 e4
    mov dx, ax                                ; 89 c2
    sal dx, 008h                              ; c1 e2 08
    mov ax, strict word 00030h                ; b8 30 00
    call 0165ch                               ; e8 a7 b4
    xor ah, ah                                ; 30 e4
    or dx, ax                                 ; 09 c2
    mov word [bp+012h], dx                    ; 89 56 12
    cmp dx, strict byte 0ffc0h                ; 83 fa c0
    jbe short 0619bh                          ; 76 da
    mov word [bp+012h], strict word 0ffc0h    ; c7 46 12 c0 ff
    jmp short 0619bh                          ; eb d3
    cli                                       ; fa
    mov ax, strict word 00001h                ; b8 01 00
    call 05db4h                               ; e8 e5 fb
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 00038h                ; 83 c2 38
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, strict word 0ffffh                ; bb ff ff
    call 0162ah                               ; e8 4c b4
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0003ah                ; 83 c2 3a
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor bx, bx                                ; 31 db
    call 0162ah                               ; e8 3e b4
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0003ch                ; 83 c2 3c
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, strict word 0000fh                ; bb 0f 00
    call 0160eh                               ; e8 13 b4
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0003dh                ; 83 c2 3d
    mov ax, word [bp+014h]                    ; 8b 46 14
    mov bx, 0009bh                            ; bb 9b 00
    call 0160eh                               ; e8 04 b4
    mov dx, word [bp+006h]                    ; 8b 56 06
    add dx, strict byte 0003eh                ; 83 c2 3e
    mov ax, word [bp+014h]                    ; 8b 46 14
    xor bx, bx                                ; 31 db
    call 0162ah                               ; e8 12 b4
    mov AL, strict byte 011h                  ; b0 11
    mov dx, strict word 00020h                ; ba 20 00
    out DX, AL                                ; ee
    mov dx, 000a0h                            ; ba a0 00
    out DX, AL                                ; ee
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    shr ax, 008h                              ; c1 e8 08
    mov dx, strict word 00021h                ; ba 21 00
    out DX, AL                                ; ee
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    mov dx, 000a1h                            ; ba a1 00
    out DX, AL                                ; ee
    mov AL, strict byte 004h                  ; b0 04
    mov dx, strict word 00021h                ; ba 21 00
    out DX, AL                                ; ee
    mov AL, strict byte 002h                  ; b0 02
    mov dx, 000a1h                            ; ba a1 00
    out DX, AL                                ; ee
    mov AL, strict byte 001h                  ; b0 01
    mov dx, strict word 00021h                ; ba 21 00
    out DX, AL                                ; ee
    mov dx, 000a1h                            ; ba a1 00
    out DX, AL                                ; ee
    mov AL, strict byte 0ffh                  ; b0 ff
    mov dx, strict word 00021h                ; ba 21 00
    out DX, AL                                ; ee
    mov dx, 000a1h                            ; ba a1 00
    out DX, AL                                ; ee
    mov si, word [bp+006h]                    ; 8b 76 06
    call 06259h                               ; e8 00 00
    pop di                                    ; 5f
    add di, strict byte 00018h                ; 83 c7 18
    push strict byte 00038h                   ; 6a 38
    push di                                   ; 57
    lgdt [es:si+008h]                         ; 26 0f 01 54 08
    lidt [es:si+010h]                         ; 26 0f 01 5c 10
    mov ax, strict word 00001h                ; b8 01 00
    lmsw ax                                   ; 0f 01 f0
    retf                                      ; cb
    mov ax, strict word 00028h                ; b8 28 00
    mov ss, ax                                ; 8e d0
    mov ax, strict word 00018h                ; b8 18 00
    mov ds, ax                                ; 8e d8
    mov ax, strict word 00020h                ; b8 20 00
    mov es, ax                                ; 8e c0
    lea ax, [bp+004h]                         ; 8d 46 04
    db  08bh, 0e0h
    ; mov sp, ax                                ; 8b e0
    popaw                                     ; 61
    add sp, strict byte 00006h                ; 83 c4 06
    pop cx                                    ; 59
    pop ax                                    ; 58
    pop ax                                    ; 58
    mov ax, strict word 00030h                ; b8 30 00
    push ax                                   ; 50
    push cx                                   ; 51
    retf                                      ; cb
    jmp near 05f4ch                           ; e9 b7 fc
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 2c b6
    push 00806h                               ; 68 06 08
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 65 b6
    add sp, strict byte 00004h                ; 83 c4 04
    or byte [bp+018h], 001h                   ; 80 4e 18 01
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    or ah, 086h                               ; 80 cc 86
    mov word [bp+012h], ax                    ; 89 46 12
    jmp near 05f4ch                           ; e9 8f fc
    mov word [bp+018h], cx                    ; 89 4e 18
    mov word [bp+012h], ax                    ; 89 46 12
    mov word [bp+00ch], 0e6f5h                ; c7 46 0c f5 e6
    mov word [bp+014h], 0f000h                ; c7 46 14 00 f0
    jmp near 05f4ch                           ; e9 7c fc
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 43 b3
    mov word [bp+014h], ax                    ; 89 46 14
    jmp near 0619bh                           ; e9 bc fe
    push 00835h                               ; 68 35 08
    push strict byte 00008h                   ; 6a 08
    jmp short 062a5h                          ; eb bf
    test byte [bp+012h], 0ffh                 ; f6 46 12 ff
    jne short 0630bh                          ; 75 1f
    mov word [bp+012h], ax                    ; 89 46 12
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 00001h                ; 3d 01 00
    jc short 06304h                           ; 72 0b
    cmp ax, strict word 00003h                ; 3d 03 00
    jnbe short 06304h                         ; 77 06
    mov word [bp+018h], cx                    ; 89 4e 18
    jmp near 05f4ch                           ; e9 48 fc
    or byte [bp+018h], 001h                   ; 80 4e 18 01
    jmp near 05f4ch                           ; e9 41 fc
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 b6 b5
    push word [bp+00ch]                       ; ff 76 0c
    push word [bp+012h]                       ; ff 76 12
    push 0084ch                               ; 68 4c 08
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 e9 b5
    add sp, strict byte 00008h                ; 83 c4 08
    jmp short 062abh                          ; eb 82
    insw                                      ; 6d
    mov word [fs:0c564h], ax                  ; 64 a3 64 c5
    db  064h, 0e6h, 064h
    ; fs out strict byte 064h, AL               ; 64 e6 64
    add ax, 02465h                            ; 05 65 24
    db  065h, 066h, 065h, 093h
    ; gs xchg ebx, eax                          ; 65 66 65 93
    db  065h
_int15_function32:                           ; 0xf6339 LB 0x30d
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    sub sp, strict byte 00008h                ; 83 ec 08
    mov ax, word [bp+020h]                    ; 8b 46 20
    shr ax, 008h                              ; c1 e8 08
    cmp ax, 000e8h                            ; 3d e8 00
    je short 0638ch                           ; 74 41
    cmp ax, 00086h                            ; 3d 86 00
    jne near 06616h                           ; 0f 85 c4 02
    sti                                       ; fb
    mov ax, word [bp+01ch]                    ; 8b 46 1c
    mov dx, word [bp+018h]                    ; 8b 56 18
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    mov ebx, strict dword 00000000fh          ; 66 bb 0f 00 00 00
    db  066h, 033h, 0d2h
    ; xor edx, edx                              ; 66 33 d2
    div ebx                                   ; 66 f7 f3
    db  066h, 08bh, 0c8h
    ; mov ecx, eax                              ; 66 8b c8
    in AL, strict byte 061h                   ; e4 61
    and AL, strict byte 010h                  ; 24 10
    db  08ah, 0e0h
    ; mov ah, al                                ; 8a e0
    db  066h, 00bh, 0c9h
    ; or ecx, ecx                               ; 66 0b c9
    je near 06389h                            ; 0f 84 0e 00
    in AL, strict byte 061h                   ; e4 61
    and AL, strict byte 010h                  ; 24 10
    db  03ah, 0c4h
    ; cmp al, ah                                ; 3a c4
    je short 0637bh                           ; 74 f8
    db  08ah, 0e0h
    ; mov ah, al                                ; 8a e0
    dec ecx                                   ; 66 49
    jne short 0637bh                          ; 75 f2
    jmp near 06640h                           ; e9 b4 02
    mov ax, word [bp+020h]                    ; 8b 46 20
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 00020h                ; 3d 20 00
    je short 063a0h                           ; 74 0a
    cmp ax, strict word 00001h                ; 3d 01 00
    je near 065c9h                            ; 0f 84 2c 02
    jmp near 06616h                           ; e9 76 02
    cmp word [bp+01ah], 0534dh                ; 81 7e 1a 4d 53
    jne near 06616h                           ; 0f 85 6d 02
    cmp word [bp+018h], 04150h                ; 81 7e 18 50 41
    jne near 06616h                           ; 0f 85 64 02
    mov ax, strict word 00035h                ; b8 35 00
    call 0165ch                               ; e8 a4 b2
    movzx bx, al                              ; 0f b6 d8
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00008h                ; b9 08 00
    sal bx, 1                                 ; d1 e3
    rcl dx, 1                                 ; d1 d2
    loop 063c0h                               ; e2 fa
    mov ax, strict word 00034h                ; b8 34 00
    call 0165ch                               ; e8 90 b2
    xor ah, ah                                ; 30 e4
    mov dx, bx                                ; 89 da
    or dx, ax                                 ; 09 c2
    xor bx, bx                                ; 31 db
    add bx, bx                                ; 01 db
    adc dx, 00100h                            ; 81 d2 00 01
    cmp dx, 00100h                            ; 81 fa 00 01
    jc short 063e6h                           ; 72 06
    jne short 06413h                          ; 75 31
    test bx, bx                               ; 85 db
    jnbe short 06413h                         ; 77 2d
    mov ax, strict word 00031h                ; b8 31 00
    call 0165ch                               ; e8 70 b2
    movzx bx, al                              ; 0f b6 d8
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00008h                ; b9 08 00
    sal bx, 1                                 ; d1 e3
    rcl dx, 1                                 ; d1 d2
    loop 063f4h                               ; e2 fa
    mov ax, strict word 00030h                ; b8 30 00
    call 0165ch                               ; e8 5c b2
    xor ah, ah                                ; 30 e4
    or bx, ax                                 ; 09 c3
    mov cx, strict word 0000ah                ; b9 0a 00
    sal bx, 1                                 ; d1 e3
    rcl dx, 1                                 ; d1 d2
    loop 06407h                               ; e2 fa
    add bx, strict byte 00000h                ; 83 c3 00
    adc dx, strict byte 00010h                ; 83 d2 10
    mov ax, strict word 00062h                ; b8 62 00
    call 0165ch                               ; e8 43 b2
    xor ah, ah                                ; 30 e4
    mov word [bp-00ah], ax                    ; 89 46 f6
    xor al, al                                ; 30 c0
    mov word [bp-008h], ax                    ; 89 46 f8
    mov cx, strict word 00008h                ; b9 08 00
    sal word [bp-00ah], 1                     ; d1 66 f6
    rcl word [bp-008h], 1                     ; d1 56 f8
    loop 06426h                               ; e2 f8
    mov ax, strict word 00061h                ; b8 61 00
    call 0165ch                               ; e8 28 b2
    xor ah, ah                                ; 30 e4
    or word [bp-00ah], ax                     ; 09 46 f6
    mov ax, word [bp-00ah]                    ; 8b 46 f6
    mov word [bp-008h], ax                    ; 89 46 f8
    mov word [bp-00ah], strict word 00000h    ; c7 46 f6 00 00
    mov ax, strict word 00063h                ; b8 63 00
    call 0165ch                               ; e8 12 b2
    mov byte [bp-006h], al                    ; 88 46 fa
    mov byte [bp-004h], al                    ; 88 46 fc
    mov ax, word [bp+014h]                    ; 8b 46 14
    cmp ax, strict word 00007h                ; 3d 07 00
    jnbe near 06616h                          ; 0f 87 bc 01
    mov si, ax                                ; 89 c6
    add si, ax                                ; 01 c6
    mov ax, bx                                ; 89 d8
    add ax, strict word 00000h                ; 05 00 00
    mov cx, dx                                ; 89 d1
    adc cx, strict byte 0ffffh                ; 83 d1 ff
    jmp word [cs:si+06329h]                   ; 2e ff a4 29 63
    push strict byte 00001h                   ; 6a 01
    push dword 000000000h                     ; 66 6a 00
    push strict byte 00009h                   ; 6a 09
    push 0fc00h                               ; 68 00 fc
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+024h]                    ; 8b 46 24
    xor bx, bx                                ; 31 db
    xor cx, cx                                ; 31 c9
    call 05de0h                               ; e8 5c f9
    mov dword [bp+014h], strict dword 000000001h ; 66 c7 46 14 01 00 00 00
    mov dword [bp+020h], strict dword 0534d4150h ; 66 c7 46 20 50 41 4d 53
    mov dword [bp+01ch], strict dword 000000014h ; 66 c7 46 1c 14 00 00 00
    and byte [bp+028h], 0feh                  ; 80 66 28 fe
    jmp near 06640h                           ; e9 9d 01
    push strict byte 00002h                   ; 6a 02
    push dword 000000000h                     ; 66 6a 00
    push strict byte 0000ah                   ; 6a 0a
    push strict byte 00000h                   ; 6a 00
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+024h]                    ; 8b 46 24
    mov bx, 0fc00h                            ; bb 00 fc
    mov cx, strict word 00009h                ; b9 09 00
    call 05de0h                               ; e8 25 f9
    mov dword [bp+014h], strict dword 000000002h ; 66 c7 46 14 02 00 00 00
    jmp short 0648ch                          ; eb c7
    push strict byte 00002h                   ; 6a 02
    push dword 000000000h                     ; 66 6a 00
    push strict byte 00010h                   ; 6a 10
    push strict byte 00000h                   ; 6a 00
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+024h]                    ; 8b 46 24
    xor bx, bx                                ; 31 db
    mov cx, strict word 0000fh                ; b9 0f 00
    call 05de0h                               ; e8 04 f9
    mov dword [bp+014h], strict dword 000000003h ; 66 c7 46 14 03 00 00 00
    jmp short 0648ch                          ; eb a6
    push strict byte 00001h                   ; 6a 01
    push dword 000000000h                     ; 66 6a 00
    push cx                                   ; 51
    push ax                                   ; 50
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+024h]                    ; 8b 46 24
    xor bx, bx                                ; 31 db
    mov cx, strict word 00010h                ; b9 10 00
    call 05de0h                               ; e8 e5 f8
    mov dword [bp+014h], strict dword 000000004h ; 66 c7 46 14 04 00 00 00
    jmp short 0648ch                          ; eb 87
    push strict byte 00003h                   ; 6a 03
    push dword 000000000h                     ; 66 6a 00
    push dx                                   ; 52
    push bx                                   ; 53
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov si, word [bp+024h]                    ; 8b 76 24
    mov bx, ax                                ; 89 c3
    mov ax, si                                ; 89 f0
    call 05de0h                               ; e8 c7 f8
    mov dword [bp+014h], strict dword 000000005h ; 66 c7 46 14 05 00 00 00
    jmp near 0648ch                           ; e9 68 ff
    push strict byte 00002h                   ; 6a 02
    push dword 000000000h                     ; 66 6a 00
    push dword 000000000h                     ; 66 6a 00
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+024h]                    ; 8b 46 24
    xor bx, bx                                ; 31 db
    mov cx, strict word 0fffch                ; b9 fc ff
    call 05de0h                               ; e8 a6 f8
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    jne short 06547h                          ; 75 07
    mov ax, word [bp-008h]                    ; 8b 46 f8
    test ax, ax                               ; 85 c0
    je short 0655dh                           ; 74 16
    mov dword [bp+014h], strict dword 000000007h ; 66 c7 46 14 07 00 00 00
    jmp near 0648ch                           ; e9 3a ff
    mov dword [bp+014h], strict dword 000000006h ; 66 c7 46 14 06 00 00 00
    jmp near 0648ch                           ; e9 2f ff
    mov word [bp+014h], ax                    ; 89 46 14
    mov word [bp+016h], ax                    ; 89 46 16
    jmp near 0648ch                           ; e9 26 ff
    push strict byte 00002h                   ; 6a 02
    push dword 000000000h                     ; 66 6a 00
    push dword 000000000h                     ; 66 6a 00
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+024h]                    ; 8b 46 24
    xor bx, bx                                ; 31 db
    xor cx, cx                                ; 31 c9
    call 05de0h                               ; e8 65 f8
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    jne short 06588h                          ; 75 07
    mov ax, word [bp-008h]                    ; 8b 46 f8
    test ax, ax                               ; 85 c0
    je short 0658ah                           ; 74 02
    jmp short 06547h                          ; eb bd
    mov word [bp+014h], ax                    ; 89 46 14
    mov word [bp+016h], ax                    ; 89 46 16
    jmp near 0648ch                           ; e9 f9 fe
    cmp byte [bp-006h], 000h                  ; 80 7e fa 00
    jne short 065a1h                          ; 75 08
    cmp word [bp-008h], strict byte 00000h    ; 83 7e f8 00
    je near 0648ch                            ; 0f 84 eb fe
    push strict byte 00001h                   ; 6a 01
    mov al, byte [bp-004h]                    ; 8a 46 fc
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    xor ah, ah                                ; 30 e4
    push ax                                   ; 50
    push strict byte 00001h                   ; 6a 01
    push dword [bp-00ah]                      ; 66 ff 76 f6
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+024h]                    ; 8b 46 24
    xor bx, bx                                ; 31 db
    xor cx, cx                                ; 31 c9
    call 05de0h                               ; e8 22 f8
    xor ax, ax                                ; 31 c0
    mov word [bp+014h], ax                    ; 89 46 14
    mov word [bp+016h], ax                    ; 89 46 16
    jmp near 0648ch                           ; e9 c3 fe
    and byte [bp+028h], 0feh                  ; 80 66 28 fe
    mov ax, strict word 00031h                ; b8 31 00
    call 0165ch                               ; e8 89 b0
    xor ah, ah                                ; 30 e4
    mov dx, ax                                ; 89 c2
    sal dx, 008h                              ; c1 e2 08
    mov ax, strict word 00030h                ; b8 30 00
    call 0165ch                               ; e8 7c b0
    xor ah, ah                                ; 30 e4
    or dx, ax                                 ; 09 c2
    mov word [bp+01ch], dx                    ; 89 56 1c
    cmp dx, 03c00h                            ; 81 fa 00 3c
    jbe short 065f2h                          ; 76 05
    mov word [bp+01ch], 03c00h                ; c7 46 1c 00 3c
    mov ax, strict word 00035h                ; b8 35 00
    call 0165ch                               ; e8 64 b0
    movzx dx, al                              ; 0f b6 d0
    sal dx, 008h                              ; c1 e2 08
    mov ax, strict word 00034h                ; b8 34 00
    call 0165ch                               ; e8 58 b0
    xor ah, ah                                ; 30 e4
    or dx, ax                                 ; 09 c2
    mov word [bp+018h], dx                    ; 89 56 18
    mov ax, word [bp+01ch]                    ; 8b 46 1c
    mov word [bp+020h], ax                    ; 89 46 20
    mov word [bp+014h], dx                    ; 89 56 14
    jmp short 06640h                          ; eb 2a
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 ab b2
    push word [bp+014h]                       ; ff 76 14
    push word [bp+020h]                       ; ff 76 20
    push 0084ch                               ; 68 4c 08
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 de b2
    add sp, strict byte 00008h                ; 83 c4 08
    or byte [bp+028h], 001h                   ; 80 4e 28 01
    mov ax, word [bp+020h]                    ; 8b 46 20
    xor al, al                                ; 30 c0
    or AL, strict byte 086h                   ; 0c 86
    mov word [bp+020h], ax                    ; 89 46 20
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
init_rtc_:                                   ; 0xf6646 LB 0x28
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    mov dx, strict word 00026h                ; ba 26 00
    mov ax, strict word 0000ah                ; b8 0a 00
    call 01670h                               ; e8 1d b0
    mov dx, strict word 00002h                ; ba 02 00
    mov ax, strict word 0000bh                ; b8 0b 00
    call 01670h                               ; e8 14 b0
    mov ax, strict word 0000ch                ; b8 0c 00
    call 0165ch                               ; e8 fa af
    mov ax, strict word 0000dh                ; b8 0d 00
    call 0165ch                               ; e8 f4 af
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
rtc_updating_:                               ; 0xf666e LB 0x21
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    mov dx, 061a8h                            ; ba a8 61
    dec dx                                    ; 4a
    je short 06686h                           ; 74 0e
    mov ax, strict word 0000ah                ; b8 0a 00
    call 0165ch                               ; e8 de af
    test AL, strict byte 080h                 ; a8 80
    jne short 06675h                          ; 75 f3
    xor ax, ax                                ; 31 c0
    jmp short 06689h                          ; eb 03
    mov ax, strict word 00001h                ; b8 01 00
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
_int70_function:                             ; 0xf668f LB 0xbe
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push ax                                   ; 50
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 c2 af
    mov dl, al                                ; 88 c2
    mov byte [bp-004h], al                    ; 88 46 fc
    mov ax, strict word 0000ch                ; b8 0c 00
    call 0165ch                               ; e8 b7 af
    mov dh, al                                ; 88 c6
    test dl, 060h                             ; f6 c2 60
    je near 06734h                            ; 0f 84 86 00
    test AL, strict byte 020h                 ; a8 20
    je short 066b6h                           ; 74 04
    sti                                       ; fb
    int 04ah                                  ; cd 4a
    cli                                       ; fa
    test dh, 040h                             ; f6 c6 40
    je near 06734h                            ; 0f 84 77 00
    mov dx, 000a0h                            ; ba a0 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 3a af
    test al, al                               ; 84 c0
    je short 06734h                           ; 74 6a
    mov dx, 0009ch                            ; ba 9c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01638h                               ; e8 65 af
    test dx, dx                               ; 85 d2
    jne short 06720h                          ; 75 49
    cmp ax, 003d1h                            ; 3d d1 03
    jnc short 06720h                          ; 73 44
    mov dx, 00098h                            ; ba 98 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 37 af
    mov si, ax                                ; 89 c6
    mov dx, 0009ah                            ; ba 9a 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 2c af
    mov cx, ax                                ; 89 c1
    xor bx, bx                                ; 31 db
    mov dx, 000a0h                            ; ba a0 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 11 af
    mov al, byte [bp-004h]                    ; 8a 46 fc
    and AL, strict byte 037h                  ; 24 37
    movzx dx, al                              ; 0f b6 d0
    mov ax, strict word 0000bh                ; b8 0b 00
    call 01670h                               ; e8 65 af
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 01600h                               ; e8 ee ae
    or AL, strict byte 080h                   ; 0c 80
    movzx bx, al                              ; 0f b6 d8
    mov dx, cx                                ; 89 ca
    mov ax, si                                ; 89 f0
    call 0160eh                               ; e8 f0 ae
    jmp short 06734h                          ; eb 14
    mov bx, ax                                ; 89 c3
    add bx, 0fc2fh                            ; 81 c3 2f fc
    mov cx, dx                                ; 89 d1
    adc cx, strict byte 0ffffh                ; 83 d1 ff
    mov dx, 0009ch                            ; ba 9c 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0164ah                               ; e8 16 af
    call 0e03bh                               ; e8 04 79
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    db  064h, 067h, 08dh, 067h, 0b2h, 067h, 0e4h, 067h, 033h, 068h, 06bh, 068h, 0aeh, 068h, 005h, 069h
_int1a_function:                             ; 0xf674d LB 0x1c8
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    sti                                       ; fb
    mov al, byte [bp+013h]                    ; 8a 46 13
    cmp AL, strict byte 007h                  ; 3c 07
    jnbe near 06789h                          ; 0f 87 2f 00
    movzx bx, al                              ; 0f b6 d8
    add bx, bx                                ; 01 db
    jmp word [cs:bx+0673dh]                   ; 2e ff a7 3d 67
    cli                                       ; fa
    mov bx, 0046eh                            ; bb 6e 04
    xor ax, ax                                ; 31 c0
    mov es, ax                                ; 8e c0
    mov ax, word [es:bx]                      ; 26 8b 07
    mov word [bp+010h], ax                    ; 89 46 10
    mov bx, 0046ch                            ; bb 6c 04
    mov ax, word [es:bx]                      ; 26 8b 07
    mov word [bp+00eh], ax                    ; 89 46 0e
    mov bx, 00470h                            ; bb 70 04
    mov al, byte [es:bx]                      ; 26 8a 07
    mov byte [bp+012h], al                    ; 88 46 12
    mov byte [es:bx], 000h                    ; 26 c6 07 00
    sti                                       ; fb
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
    cli                                       ; fa
    mov bx, 0046eh                            ; bb 6e 04
    xor ax, ax                                ; 31 c0
    mov es, ax                                ; 8e c0
    mov ax, word [bp+010h]                    ; 8b 46 10
    mov word [es:bx], ax                      ; 26 89 07
    mov bx, 0046ch                            ; bb 6c 04
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    mov word [es:bx], ax                      ; 26 89 07
    mov bx, 00470h                            ; bb 70 04
    mov byte [es:bx], 000h                    ; 26 c6 07 00
    sti                                       ; fb
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    jmp short 06789h                          ; eb d7
    call 0666eh                               ; e8 b9 fe
    test ax, ax                               ; 85 c0
    je short 067bbh                           ; 74 02
    jmp short 06789h                          ; eb ce
    xor ax, ax                                ; 31 c0
    call 0165ch                               ; e8 9c ae
    mov byte [bp+00fh], al                    ; 88 46 0f
    mov ax, strict word 00002h                ; b8 02 00
    call 0165ch                               ; e8 93 ae
    mov byte [bp+010h], al                    ; 88 46 10
    mov ax, strict word 00004h                ; b8 04 00
    call 0165ch                               ; e8 8a ae
    mov bl, al                                ; 88 c3
    mov byte [bp+011h], al                    ; 88 46 11
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 7f ae
    and AL, strict byte 001h                  ; 24 01
    mov byte [bp+00eh], al                    ; 88 46 0e
    jmp short 06829h                          ; eb 45
    call 0666eh                               ; e8 87 fe
    test ax, ax                               ; 85 c0
    je short 067eeh                           ; 74 03
    call 06646h                               ; e8 58 fe
    movzx dx, byte [bp+00fh]                  ; 0f b6 56 0f
    xor ax, ax                                ; 31 c0
    call 01670h                               ; e8 79 ae
    movzx dx, byte [bp+010h]                  ; 0f b6 56 10
    mov ax, strict word 00002h                ; b8 02 00
    call 01670h                               ; e8 6f ae
    movzx dx, byte [bp+011h]                  ; 0f b6 56 11
    mov ax, strict word 00004h                ; b8 04 00
    call 01670h                               ; e8 65 ae
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 4b ae
    mov bl, al                                ; 88 c3
    and bl, 060h                              ; 80 e3 60
    or bl, 002h                               ; 80 cb 02
    mov al, byte [bp+00eh]                    ; 8a 46 0e
    and AL, strict byte 001h                  ; 24 01
    or bl, al                                 ; 08 c3
    movzx dx, bl                              ; 0f b6 d3
    mov ax, strict word 0000bh                ; b8 0b 00
    call 01670h                               ; e8 47 ae
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    mov byte [bp+012h], bl                    ; 88 5e 12
    jmp near 06789h                           ; e9 56 ff
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    call 0666eh                               ; e8 34 fe
    test ax, ax                               ; 85 c0
    je short 06841h                           ; 74 03
    jmp near 06789h                           ; e9 48 ff
    mov ax, strict word 00009h                ; b8 09 00
    call 0165ch                               ; e8 15 ae
    mov byte [bp+010h], al                    ; 88 46 10
    mov ax, strict word 00008h                ; b8 08 00
    call 0165ch                               ; e8 0c ae
    mov byte [bp+00fh], al                    ; 88 46 0f
    mov ax, strict word 00007h                ; b8 07 00
    call 0165ch                               ; e8 03 ae
    mov byte [bp+00eh], al                    ; 88 46 0e
    mov ax, strict word 00032h                ; b8 32 00
    call 0165ch                               ; e8 fa ad
    mov byte [bp+011h], al                    ; 88 46 11
    mov byte [bp+012h], al                    ; 88 46 12
    jmp near 06789h                           ; e9 1e ff
    call 0666eh                               ; e8 00 fe
    test ax, ax                               ; 85 c0
    je short 06878h                           ; 74 06
    call 06646h                               ; e8 d1 fd
    jmp near 06789h                           ; e9 11 ff
    movzx dx, byte [bp+010h]                  ; 0f b6 56 10
    mov ax, strict word 00009h                ; b8 09 00
    call 01670h                               ; e8 ee ad
    movzx dx, byte [bp+00fh]                  ; 0f b6 56 0f
    mov ax, strict word 00008h                ; b8 08 00
    call 01670h                               ; e8 e4 ad
    movzx dx, byte [bp+00eh]                  ; 0f b6 56 0e
    mov ax, strict word 00007h                ; b8 07 00
    call 01670h                               ; e8 da ad
    movzx dx, byte [bp+011h]                  ; 0f b6 56 11
    mov ax, strict word 00032h                ; b8 32 00
    call 01670h                               ; e8 d0 ad
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 b6 ad
    mov bl, al                                ; 88 c3
    and bl, 07fh                              ; 80 e3 7f
    jmp near 06820h                           ; e9 72 ff
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 a8 ad
    mov bl, al                                ; 88 c3
    mov word [bp+012h], strict word 00000h    ; c7 46 12 00 00
    test AL, strict byte 020h                 ; a8 20
    je short 068c2h                           ; 74 03
    jmp near 06789h                           ; e9 c7 fe
    call 0666eh                               ; e8 a9 fd
    test ax, ax                               ; 85 c0
    je short 068cch                           ; 74 03
    call 06646h                               ; e8 7a fd
    movzx dx, byte [bp+00fh]                  ; 0f b6 56 0f
    mov ax, strict word 00001h                ; b8 01 00
    call 01670h                               ; e8 9a ad
    movzx dx, byte [bp+010h]                  ; 0f b6 56 10
    mov ax, strict word 00003h                ; b8 03 00
    call 01670h                               ; e8 90 ad
    movzx dx, byte [bp+011h]                  ; 0f b6 56 11
    mov ax, strict word 00005h                ; b8 05 00
    call 01670h                               ; e8 86 ad
    mov dx, 000a1h                            ; ba a1 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 0feh                  ; 24 fe
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    and AL, strict byte 05fh                  ; 24 5f
    or AL, strict byte 020h                   ; 0c 20
    movzx dx, al                              ; 0f b6 d0
    mov ax, strict word 0000bh                ; b8 0b 00
    call 01670h                               ; e8 6e ad
    jmp near 06789h                           ; e9 84 fe
    mov ax, strict word 0000bh                ; b8 0b 00
    call 0165ch                               ; e8 51 ad
    mov bl, al                                ; 88 c3
    and AL, strict byte 057h                  ; 24 57
    movzx dx, al                              ; 0f b6 d0
    jmp near 06823h                           ; e9 0e ff
send_to_mouse_ctrl_:                         ; 0xf6915 LB 0x34
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bl, al                                ; 88 c3
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 06934h                           ; 74 0e
    push 00886h                               ; 68 86 08
    push 010a0h                               ; 68 a0 10
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 dc af
    add sp, strict byte 00006h                ; 83 c4 06
    mov AL, strict byte 0d4h                  ; b0 d4
    mov dx, strict word 00064h                ; ba 64 00
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    xor al, bl                                ; 30 d8
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
get_mouse_data_:                             ; 0xf6949 LB 0x3b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    mov bx, ax                                ; 89 c3
    mov es, dx                                ; 8e c2
    mov cx, 02710h                            ; b9 10 27
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and ax, strict word 00021h                ; 25 21 00
    cmp ax, strict word 00021h                ; 3d 21 00
    je short 0696ah                           ; 74 07
    test cx, cx                               ; 85 c9
    je short 0696ah                           ; 74 03
    dec cx                                    ; 49
    jmp short 06955h                          ; eb eb
    test cx, cx                               ; 85 c9
    jne short 06972h                          ; 75 04
    mov AL, strict byte 001h                  ; b0 01
    jmp short 0697dh                          ; eb 0b
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [es:bx], al                      ; 26 88 07
    xor al, al                                ; 30 c0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
set_kbd_command_byte_:                       ; 0xf6984 LB 0x32
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bl, al                                ; 88 c3
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 002h                 ; a8 02
    je short 069a3h                           ; 74 0e
    push 00890h                               ; 68 90 08
    push 010a0h                               ; 68 a0 10
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 6d af
    add sp, strict byte 00006h                ; 83 c4 06
    mov AL, strict byte 060h                  ; b0 60
    mov dx, strict word 00064h                ; ba 64 00
    out DX, AL                                ; ee
    mov al, bl                                ; 88 d8
    mov dx, strict word 00060h                ; ba 60 00
    out DX, AL                                ; ee
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
_int74_function:                             ; 0xf69b6 LB 0xca
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    sub sp, strict byte 00008h                ; 83 ec 08
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 57 ac
    mov cx, ax                                ; 89 c1
    mov word [bp+004h], strict word 00000h    ; c7 46 04 00 00
    mov dx, strict word 00064h                ; ba 64 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    and AL, strict byte 021h                  ; 24 21
    cmp AL, strict byte 021h                  ; 3c 21
    jne near 06a6ch                           ; 0f 85 92 00
    mov dx, strict word 00060h                ; ba 60 00
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov bl, al                                ; 88 c3
    mov dx, strict word 00026h                ; ba 26 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 16 ac
    mov byte [bp-006h], al                    ; 88 46 fa
    mov dx, strict word 00027h                ; ba 27 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 0b ac
    mov byte [bp-008h], al                    ; 88 46 f8
    test AL, strict byte 080h                 ; a8 80
    je short 06a6ch                           ; 74 70
    mov al, byte [bp-008h]                    ; 8a 46 f8
    and AL, strict byte 007h                  ; 24 07
    mov byte [bp-002h], al                    ; 88 46 fe
    mov al, byte [bp-006h]                    ; 8a 46 fa
    and AL, strict byte 007h                  ; 24 07
    mov byte [bp-004h], al                    ; 88 46 fc
    xor bh, bh                                ; 30 ff
    movzx dx, al                              ; 0f b6 d0
    add dx, strict byte 00028h                ; 83 c2 28
    mov ax, cx                                ; 89 c8
    call 0160eh                               ; e8 f5 ab
    mov al, byte [bp-004h]                    ; 8a 46 fc
    cmp al, byte [bp-002h]                    ; 3a 46 fe
    jc short 06a5dh                           ; 72 3c
    mov dx, strict word 00028h                ; ba 28 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 d7 ab
    xor ah, ah                                ; 30 e4
    mov word [bp+00ch], ax                    ; 89 46 0c
    mov dx, strict word 00029h                ; ba 29 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 ca ab
    xor ah, ah                                ; 30 e4
    mov word [bp+00ah], ax                    ; 89 46 0a
    mov dx, strict word 0002ah                ; ba 2a 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 bd ab
    xor ah, ah                                ; 30 e4
    mov word [bp+008h], ax                    ; 89 46 08
    xor al, al                                ; 30 c0
    mov word [bp+006h], ax                    ; 89 46 06
    mov byte [bp-006h], ah                    ; 88 66 fa
    test byte [bp-008h], 080h                 ; f6 46 f8 80
    je short 06a60h                           ; 74 0a
    mov word [bp+004h], strict word 00001h    ; c7 46 04 01 00
    jmp short 06a60h                          ; eb 03
    inc byte [bp-006h]                        ; fe 46 fa
    movzx bx, byte [bp-006h]                  ; 0f b6 5e fa
    mov dx, strict word 00026h                ; ba 26 00
    mov ax, cx                                ; 89 c8
    call 0160eh                               ; e8 a2 ab
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
    les bp, [bp+si+03ah]                      ; c4 6a 3a
    imul di, word [di+04e6bh], strict byte 0006ch ; 6b bd 6b 4e 6c
    mov sp, 0106ch                            ; bc 6c 10
    imul sp, sp, strict byte 0006ch           ; 6b e4 6c
    db  0a9h
    insw                                      ; 6d
_int15_function_mouse:                       ; 0xf6a80 LB 0x38b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    sub sp, strict byte 00006h                ; 83 ec 06
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 8c ab
    mov cx, ax                                ; 89 c1
    cmp byte [bp+012h], 007h                  ; 80 7e 12 07
    jbe short 06aa3h                          ; 76 0b
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 001h                  ; c6 46 13 01
    jmp near 06e05h                           ; e9 62 03
    mov ax, strict word 00065h                ; b8 65 00
    call 06984h                               ; e8 db fe
    and word [bp+018h], strict byte 0fffeh    ; 83 66 18 fe
    mov byte [bp+013h], 000h                  ; c6 46 13 00
    mov al, byte [bp+012h]                    ; 8a 46 12
    cmp AL, strict byte 007h                  ; 3c 07
    jnbe near 06dech                          ; 0f 87 32 03
    movzx si, al                              ; 0f b6 f0
    add si, si                                ; 01 f6
    jmp word [cs:si+06a70h]                   ; 2e ff a4 70 6a
    cmp byte [bp+00dh], 001h                  ; 80 7e 0d 01
    jnbe near 06df7h                          ; 0f 87 2b 03
    mov dx, strict word 00027h                ; ba 27 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 2c ab
    test AL, strict byte 080h                 ; a8 80
    jne short 06ae3h                          ; 75 0b
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 005h                  ; c6 46 13 05
    jmp near 06dffh                           ; e9 1c 03
    cmp byte [bp+00dh], 000h                  ; 80 7e 0d 00
    db  00fh, 094h, 0c0h
    ; sete al                                   ; 0f 94 c0
    add AL, strict byte 0f4h                  ; 04 f4
    xor ah, ah                                ; 30 e4
    call 06915h                               ; e8 24 fe
    test al, al                               ; 84 c0
    jne near 06d85h                           ; 0f 85 8e 02
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 4a fe
    test al, al                               ; 84 c0
    je near 06dffh                            ; 0f 84 fa 02
    cmp byte [bp-008h], 0fah                  ; 80 7e f8 fa
    jne near 06d85h                           ; 0f 85 78 02
    jmp near 06dffh                           ; e9 ef 02
    mov al, byte [bp+00dh]                    ; 8a 46 0d
    cmp AL, strict byte 001h                  ; 3c 01
    jc short 06b1bh                           ; 72 04
    cmp AL, strict byte 008h                  ; 3c 08
    jbe short 06b1eh                          ; 76 03
    jmp near 06cb1h                           ; e9 93 01
    mov dx, strict word 00027h                ; ba 27 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 da aa
    mov ah, byte [bp+00dh]                    ; 8a 66 0d
    db  0feh, 0cch
    ; dec ah                                    ; fe cc
    and AL, strict byte 0f8h                  ; 24 f8
    or al, ah                                 ; 08 e0
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00027h                ; ba 27 00
    mov ax, cx                                ; 89 c8
    call 0160eh                               ; e8 d4 aa
    mov dx, strict word 00026h                ; ba 26 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 be aa
    and AL, strict byte 0f8h                  ; 24 f8
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00026h                ; ba 26 00
    mov ax, cx                                ; 89 c8
    call 0160eh                               ; e8 bf aa
    mov ax, 000ffh                            ; b8 ff 00
    call 06915h                               ; e8 c0 fd
    test al, al                               ; 84 c0
    jne near 06d85h                           ; 0f 85 2a 02
    mov dx, ss                                ; 8c d2
    lea ax, [bp-004h]                         ; 8d 46 fc
    call 06949h                               ; e8 e6 fd
    mov cl, al                                ; 88 c1
    cmp byte [bp-004h], 0feh                  ; 80 7e fc fe
    jne short 06b76h                          ; 75 0b
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 004h                  ; c6 46 13 04
    jmp near 06dffh                           ; e9 89 02
    cmp byte [bp-004h], 0fah                  ; 80 7e fc fa
    je short 06b8ch                           ; 74 10
    movzx ax, byte [bp-004h]                  ; 0f b6 46 fc
    push ax                                   ; 50
    push 0089bh                               ; 68 9b 08
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 84 ad
    add sp, strict byte 00006h                ; 83 c4 06
    test cl, cl                               ; 84 c9
    jne near 06d85h                           ; 0f 85 f3 01
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 af fd
    test al, al                               ; 84 c0
    jne near 06d85h                           ; 0f 85 e5 01
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 06949h                               ; e8 a1 fd
    test al, al                               ; 84 c0
    jne near 06d85h                           ; 0f 85 d7 01
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov byte [bp+00ch], al                    ; 88 46 0c
    mov al, byte [bp-006h]                    ; 8a 46 fa
    mov byte [bp+00dh], al                    ; 88 46 0d
    jmp near 06dffh                           ; e9 42 02
    mov al, byte [bp+00dh]                    ; 8a 46 0d
    cmp AL, strict byte 003h                  ; 3c 03
    jc short 06bd4h                           ; 72 10
    jbe short 06bf2h                          ; 76 2c
    cmp AL, strict byte 006h                  ; 3c 06
    je short 06c04h                           ; 74 3a
    cmp AL, strict byte 005h                  ; 3c 05
    je short 06bfeh                           ; 74 30
    cmp AL, strict byte 004h                  ; 3c 04
    je short 06bf8h                           ; 74 26
    jmp short 06c0ah                          ; eb 36
    cmp AL, strict byte 002h                  ; 3c 02
    je short 06bech                           ; 74 14
    cmp AL, strict byte 001h                  ; 3c 01
    je short 06be6h                           ; 74 0a
    test al, al                               ; 84 c0
    jne short 06c0ah                          ; 75 2a
    mov byte [bp-008h], 00ah                  ; c6 46 f8 0a
    jmp short 06c0eh                          ; eb 28
    mov byte [bp-008h], 014h                  ; c6 46 f8 14
    jmp short 06c0eh                          ; eb 22
    mov byte [bp-008h], 028h                  ; c6 46 f8 28
    jmp short 06c0eh                          ; eb 1c
    mov byte [bp-008h], 03ch                  ; c6 46 f8 3c
    jmp short 06c0eh                          ; eb 16
    mov byte [bp-008h], 050h                  ; c6 46 f8 50
    jmp short 06c0eh                          ; eb 10
    mov byte [bp-008h], 064h                  ; c6 46 f8 64
    jmp short 06c0eh                          ; eb 0a
    mov byte [bp-008h], 0c8h                  ; c6 46 f8 c8
    jmp short 06c0eh                          ; eb 04
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    cmp byte [bp-008h], 000h                  ; 80 7e f8 00
    jbe short 06c43h                          ; 76 2f
    mov ax, 000f3h                            ; b8 f3 00
    call 06915h                               ; e8 fb fc
    test al, al                               ; 84 c0
    jne short 06c38h                          ; 75 1a
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 06949h                               ; e8 23 fd
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    call 06915h                               ; e8 e8 fc
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 06949h                               ; e8 14 fd
    jmp near 06dffh                           ; e9 c7 01
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 003h                  ; c6 46 13 03
    jmp near 06dffh                           ; e9 bc 01
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 002h                  ; c6 46 13 02
    jmp near 06dffh                           ; e9 b1 01
    cmp byte [bp+00dh], 004h                  ; 80 7e 0d 04
    jnc short 06cb1h                          ; 73 5d
    mov ax, 000e8h                            ; b8 e8 00
    call 06915h                               ; e8 bb fc
    test al, al                               ; 84 c0
    jne short 06ca6h                          ; 75 48
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 e3 fc
    cmp byte [bp-008h], 0fah                  ; 80 7e f8 fa
    je short 06c7ch                           ; 74 10
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    push 008c6h                               ; 68 c6 08
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 94 ac
    add sp, strict byte 00006h                ; 83 c4 06
    movzx ax, byte [bp+00dh]                  ; 0f b6 46 0d
    call 06915h                               ; e8 92 fc
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 be fc
    cmp byte [bp-008h], 0fah                  ; 80 7e f8 fa
    je near 06dffh                            ; 0f 84 6c 01
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    push 008c6h                               ; 68 c6 08
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 6d ac
    add sp, strict byte 00006h                ; 83 c4 06
    jmp near 06dffh                           ; e9 59 01
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 003h                  ; c6 46 13 03
    jmp near 06dffh                           ; e9 4e 01
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 002h                  ; c6 46 13 02
    jmp near 06dffh                           ; e9 43 01
    mov ax, 000f2h                            ; b8 f2 00
    call 06915h                               ; e8 53 fc
    test al, al                               ; 84 c0
    jne short 06cd9h                          ; 75 13
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 7b fc
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 06949h                               ; e8 73 fc
    jmp near 06bb4h                           ; e9 db fe
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 003h                  ; c6 46 13 03
    jmp near 06dffh                           ; e9 1b 01
    mov al, byte [bp+00dh]                    ; 8a 46 0d
    test al, al                               ; 84 c0
    jbe short 06cf2h                          ; 76 07
    cmp AL, strict byte 002h                  ; 3c 02
    jbe short 06d5bh                          ; 76 6c
    jmp near 06d8fh                           ; e9 9d 00
    mov ax, 000e9h                            ; b8 e9 00
    call 06915h                               ; e8 1d fc
    test al, al                               ; 84 c0
    jne near 06d85h                           ; 0f 85 87 00
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 43 fc
    mov cl, al                                ; 88 c1
    cmp byte [bp-008h], 0fah                  ; 80 7e f8 fa
    je short 06d1eh                           ; 74 10
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    push 008c6h                               ; 68 c6 08
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 f2 ab
    add sp, strict byte 00006h                ; 83 c4 06
    test cl, cl                               ; 84 c9
    jne short 06d85h                          ; 75 63
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 1f fc
    test al, al                               ; 84 c0
    jne short 06d85h                          ; 75 57
    mov dx, ss                                ; 8c d2
    lea ax, [bp-006h]                         ; 8d 46 fa
    call 06949h                               ; e8 13 fc
    test al, al                               ; 84 c0
    jne short 06d85h                          ; 75 4b
    mov dx, ss                                ; 8c d2
    lea ax, [bp-004h]                         ; 8d 46 fc
    call 06949h                               ; e8 07 fc
    test al, al                               ; 84 c0
    jne short 06d85h                          ; 75 3f
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov byte [bp+00ch], al                    ; 88 46 0c
    mov al, byte [bp-006h]                    ; 8a 46 fa
    mov byte [bp+010h], al                    ; 88 46 10
    mov al, byte [bp-004h]                    ; 8a 46 fc
    mov byte [bp+00eh], al                    ; 88 46 0e
    jmp near 06dffh                           ; e9 a4 00
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 06d64h                          ; 75 05
    mov ax, 000e6h                            ; b8 e6 00
    jmp short 06d67h                          ; eb 03
    mov ax, 000e7h                            ; b8 e7 00
    call 06915h                               ; e8 ab fb
    mov cl, al                                ; 88 c1
    test cl, cl                               ; 84 c9
    jne short 06d7fh                          ; 75 0f
    mov dx, ss                                ; 8c d2
    lea ax, [bp-008h]                         ; 8d 46 f8
    call 06949h                               ; e8 d1 fb
    cmp byte [bp-008h], 0fah                  ; 80 7e f8 fa
    db  00fh, 095h, 0c1h
    ; setne cl                                  ; 0f 95 c1
    test cl, cl                               ; 84 c9
    je near 06dffh                            ; 0f 84 7a 00
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 003h                  ; c6 46 13 03
    jmp short 06dffh                          ; eb 70
    movzx ax, byte [bp+00dh]                  ; 0f b6 46 0d
    push ax                                   ; 50
    push 008f2h                               ; 68 f2 08
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 71 ab
    add sp, strict byte 00006h                ; 83 c4 06
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 001h                  ; c6 46 13 01
    jmp short 06dffh                          ; eb 56
    mov si, word [bp+00ch]                    ; 8b 76 0c
    mov bx, si                                ; 89 f3
    mov dx, strict word 00022h                ; ba 22 00
    mov ax, cx                                ; 89 c8
    call 0162ah                               ; e8 74 a8
    mov bx, word [bp+014h]                    ; 8b 5e 14
    mov dx, strict word 00024h                ; ba 24 00
    mov ax, cx                                ; 89 c8
    call 0162ah                               ; e8 69 a8
    mov dx, strict word 00027h                ; ba 27 00
    mov ax, cx                                ; 89 c8
    call 01600h                               ; e8 37 a8
    mov ah, al                                ; 88 c4
    test si, si                               ; 85 f6
    jne short 06dddh                          ; 75 0e
    cmp word [bp+014h], strict byte 00000h    ; 83 7e 14 00
    jne short 06dddh                          ; 75 08
    test AL, strict byte 080h                 ; a8 80
    je short 06ddfh                           ; 74 06
    and AL, strict byte 07fh                  ; 24 7f
    jmp short 06ddfh                          ; eb 02
    or AL, strict byte 080h                   ; 0c 80
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00027h                ; ba 27 00
    mov ax, cx                                ; 89 c8
    call 0160eh                               ; e8 24 a8
    jmp short 06dffh                          ; eb 13
    push 0090ch                               ; 68 0c 09
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 19 ab
    add sp, strict byte 00004h                ; 83 c4 04
    or word [bp+018h], strict byte 00001h     ; 83 4e 18 01
    mov byte [bp+013h], 001h                  ; c6 46 13 01
    mov ax, strict word 00047h                ; b8 47 00
    call 06984h                               ; e8 7f fb
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
_int17_function:                             ; 0xf6e0b LB 0xb3
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push ax                                   ; 50
    sti                                       ; fb
    mov dx, word [bp+00eh]                    ; 8b 56 0e
    add dx, dx                                ; 01 d2
    add dx, strict byte 00008h                ; 83 c2 08
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 fd a7
    mov bx, ax                                ; 89 c3
    mov si, ax                                ; 89 c6
    cmp byte [bp+013h], 003h                  ; 80 7e 13 03
    jnc near 06eb4h                           ; 0f 83 89 00
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    cmp ax, strict word 00003h                ; 3d 03 00
    jnc near 06eb4h                           ; 0f 83 7f 00
    test bx, bx                               ; 85 db
    jbe near 06eb4h                           ; 0f 86 79 00
    mov dx, ax                                ; 89 c2
    add dx, strict byte 00078h                ; 83 c2 78
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 ba a7
    movzx cx, al                              ; 0f b6 c8
    sal cx, 008h                              ; c1 e1 08
    cmp byte [bp+013h], 000h                  ; 80 7e 13 00
    jne short 06e7fh                          ; 75 2d
    mov al, byte [bp+012h]                    ; 8a 46 12
    mov dx, bx                                ; 89 da
    out DX, AL                                ; ee
    lea dx, [bx+002h]                         ; 8d 57 02
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-004h], ax                    ; 89 46 fc
    mov al, byte [bp-004h]                    ; 8a 46 fc
    or AL, strict byte 001h                   ; 0c 01
    out DX, AL                                ; ee
    movzx ax, byte [bp-004h]                  ; 0f b6 46 fc
    and AL, strict byte 0feh                  ; 24 fe
    out DX, AL                                ; ee
    lea dx, [si+001h]                         ; 8d 54 01
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 040h                 ; a8 40
    je short 06e7fh                           ; 74 07
    test cx, cx                               ; 85 c9
    je short 06e7fh                           ; 74 03
    dec cx                                    ; 49
    jmp short 06e6eh                          ; eb ef
    cmp byte [bp+013h], 001h                  ; 80 7e 13 01
    jne short 06e9bh                          ; 75 16
    lea dx, [si+002h]                         ; 8d 54 02
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov word [bp-004h], ax                    ; 89 46 fc
    movzx ax, byte [bp-004h]                  ; 0f b6 46 fc
    and AL, strict byte 0fbh                  ; 24 fb
    out DX, AL                                ; ee
    mov al, byte [bp-004h]                    ; 8a 46 fc
    or AL, strict byte 004h                   ; 0c 04
    out DX, AL                                ; ee
    lea dx, [si+001h]                         ; 8d 54 01
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    xor AL, strict byte 048h                  ; 34 48
    mov byte [bp+013h], al                    ; 88 46 13
    test cx, cx                               ; 85 c9
    jne short 06eaeh                          ; 75 04
    or byte [bp+013h], 001h                   ; 80 4e 13 01
    and byte [bp+01ch], 0feh                  ; 80 66 1c fe
    jmp short 06eb8h                          ; eb 04
    or byte [bp+01ch], 001h                   ; 80 4e 1c 01
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
wait_:                                       ; 0xf6ebe LB 0xb2
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ah                ; 83 ec 0a
    mov si, ax                                ; 89 c6
    mov byte [bp-00ch], dl                    ; 88 56 f4
    mov byte [bp-00ah], 000h                  ; c6 46 f6 00
    pushfw                                    ; 9c
    pop ax                                    ; 58
    mov word [bp-010h], ax                    ; 89 46 f0
    sti                                       ; fb
    xor cx, cx                                ; 31 c9
    mov dx, 0046ch                            ; ba 6c 04
    xor ax, ax                                ; 31 c0
    call 01638h                               ; e8 57 a7
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov bx, dx                                ; 89 d3
    hlt                                       ; f4
    mov dx, 0046ch                            ; ba 6c 04
    xor ax, ax                                ; 31 c0
    call 01638h                               ; e8 49 a7
    mov word [bp-012h], ax                    ; 89 46 ee
    mov di, dx                                ; 89 d7
    cmp dx, bx                                ; 39 da
    jnbe short 06effh                         ; 77 07
    jne short 06f06h                          ; 75 0c
    cmp ax, word [bp-00eh]                    ; 3b 46 f2
    jbe short 06f06h                          ; 76 07
    sub ax, word [bp-00eh]                    ; 2b 46 f2
    sbb dx, bx                                ; 19 da
    jmp short 06f11h                          ; eb 0b
    cmp dx, bx                                ; 39 da
    jc short 06f11h                           ; 72 07
    jne short 06f15h                          ; 75 09
    cmp ax, word [bp-00eh]                    ; 3b 46 f2
    jnc short 06f15h                          ; 73 04
    sub si, ax                                ; 29 c6
    sbb cx, dx                                ; 19 d1
    mov ax, word [bp-012h]                    ; 8b 46 ee
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov bx, di                                ; 89 fb
    mov ax, 00100h                            ; b8 00 01
    int 016h                                  ; cd 16
    je near 06f2bh                            ; 0f 84 05 00
    mov AL, strict byte 001h                  ; b0 01
    jmp near 06f2dh                           ; e9 02 00
    db  032h, 0c0h
    ; xor al, al                                ; 32 c0
    test al, al                               ; 84 c0
    je short 06f55h                           ; 74 24
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    int 016h                                  ; cd 16
    xchg ah, al                               ; 86 c4
    mov dl, al                                ; 88 c2
    mov byte [bp-00ah], al                    ; 88 46 f6
    movzx ax, dl                              ; 0f b6 c2
    push ax                                   ; 50
    push 0092eh                               ; 68 2e 09
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 c5 a9
    add sp, strict byte 00006h                ; 83 c4 06
    cmp byte [bp-00ch], 000h                  ; 80 7e f4 00
    je short 06f55h                           ; 74 04
    mov al, dl                                ; 88 d0
    jmp short 06f67h                          ; eb 12
    test cx, cx                               ; 85 c9
    jnle short 06ee6h                         ; 7f 8d
    jne short 06f5fh                          ; 75 04
    test si, si                               ; 85 f6
    jnbe short 06ee6h                         ; 77 87
    mov ax, word [bp-010h]                    ; 8b 46 f0
    push ax                                   ; 50
    popfw                                     ; 9d
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
read_logo_byte_:                             ; 0xf6f70 LB 0x16
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov dx, 003b8h                            ; ba b8 03
    out DX, ax                                ; ef
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
read_logo_word_:                             ; 0xf6f86 LB 0x14
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push dx                                   ; 52
    xor ah, ah                                ; 30 e4
    or ah, 001h                               ; 80 cc 01
    mov dx, 003b8h                            ; ba b8 03
    out DX, ax                                ; ef
    in ax, DX                                 ; ed
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop dx                                    ; 5a
    pop bp                                    ; 5d
    retn                                      ; c3
print_detected_harddisks_:                   ; 0xf6f9a LB 0x130
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 6f a6
    mov si, ax                                ; 89 c6
    mov byte [bp-00ch], 000h                  ; c6 46 f4 00
    xor cl, cl                                ; 30 c9
    xor ch, ch                                ; 30 ed
    mov dx, 002c0h                            ; ba c0 02
    call 01600h                               ; e8 43 a6
    mov byte [bp-00eh], al                    ; 88 46 f2
    xor bl, bl                                ; 30 db
    cmp bl, byte [bp-00eh]                    ; 3a 5e f2
    jnc near 0709ch                           ; 0f 83 d3 00
    movzx dx, bl                              ; 0f b6 d3
    add dx, 002c1h                            ; 81 c2 c1 02
    mov ax, si                                ; 89 f0
    call 01600h                               ; e8 2b a6
    mov bh, al                                ; 88 c7
    cmp AL, strict byte 00ch                  ; 3c 0c
    jc short 06fffh                           ; 72 24
    test cl, cl                               ; 84 c9
    jne short 06fech                          ; 75 0d
    push 0093fh                               ; 68 3f 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 26 a9
    add sp, strict byte 00004h                ; 83 c4 04
    mov CL, strict byte 001h                  ; b1 01
    movzx ax, bl                              ; 0f b6 c3
    inc ax                                    ; 40
    push ax                                   ; 50
    push 00953h                               ; 68 53 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 14 a9
    add sp, strict byte 00006h                ; 83 c4 06
    jmp near 07097h                           ; e9 98 00
    cmp AL, strict byte 008h                  ; 3c 08
    jc short 07016h                           ; 72 13
    test ch, ch                               ; 84 ed
    jne short 07014h                          ; 75 0d
    push 00966h                               ; 68 66 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 fe a8
    add sp, strict byte 00004h                ; 83 c4 04
    mov CH, strict byte 001h                  ; b5 01
    jmp short 06fech                          ; eb d6
    cmp AL, strict byte 004h                  ; 3c 04
    jnc short 07031h                          ; 73 17
    cmp byte [bp-00ch], 000h                  ; 80 7e f4 00
    jne short 07031h                          ; 75 11
    push 0097ah                               ; 68 7a 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 e5 a8
    add sp, strict byte 00004h                ; 83 c4 04
    mov byte [bp-00ch], 001h                  ; c6 46 f4 01
    jmp short 07047h                          ; eb 16
    cmp bh, 004h                              ; 80 ff 04
    jc short 07047h                           ; 72 11
    test cl, cl                               ; 84 c9
    jne short 07047h                          ; 75 0d
    push 0093fh                               ; 68 3f 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 cb a8
    add sp, strict byte 00004h                ; 83 c4 04
    mov CL, strict byte 001h                  ; b1 01
    movzx ax, bl                              ; 0f b6 c3
    inc ax                                    ; 40
    push ax                                   ; 50
    push 0098bh                               ; 68 8b 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 b9 a8
    add sp, strict byte 00006h                ; 83 c4 06
    cmp bh, 004h                              ; 80 ff 04
    jc short 0705fh                           ; 72 03
    sub bh, 004h                              ; 80 ef 04
    movzx ax, bh                              ; 0f b6 c7
    cwd                                       ; 99
    db  02bh, 0c2h
    ; sub ax, dx                                ; 2b c2
    sar ax, 1                                 ; d1 f8
    test ax, ax                               ; 85 c0
    je short 07070h                           ; 74 05
    push 00995h                               ; 68 95 09
    jmp short 07073h                          ; eb 03
    push 009a0h                               ; 68 a0 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 95 a8
    add sp, strict byte 00004h                ; 83 c4 04
    movzx ax, bh                              ; 0f b6 c7
    mov di, strict word 00002h                ; bf 02 00
    cwd                                       ; 99
    idiv di                                   ; f7 ff
    test dx, dx                               ; 85 d2
    je short 0708dh                           ; 74 05
    push 009a9h                               ; 68 a9 09
    jmp short 07090h                          ; eb 03
    push 009afh                               ; 68 af 09
    push di                                   ; 57
    call 0190dh                               ; e8 79 a8
    add sp, strict byte 00004h                ; 83 c4 04
    db  0feh, 0c3h
    ; inc bl                                    ; fe c3
    jmp near 06fc2h                           ; e9 26 ff
    cmp byte [bp-00ch], 000h                  ; 80 7e f4 00
    jne short 070b5h                          ; 75 13
    test cl, cl                               ; 84 c9
    jne short 070b5h                          ; 75 0f
    test ch, ch                               ; 84 ed
    jne short 070b5h                          ; 75 0b
    push 009b6h                               ; 68 b6 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 5b a8
    add sp, strict byte 00004h                ; 83 c4 04
    push 009cah                               ; 68 ca 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 50 a8
    add sp, strict byte 00004h                ; 83 c4 04
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
get_boot_drive_:                             ; 0xf70ca LB 0x28
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov bl, al                                ; 88 c3
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 42 a5
    mov dx, 002c0h                            ; ba c0 02
    call 01600h                               ; e8 20 a5
    sub bl, 002h                              ; 80 eb 02
    cmp bl, al                                ; 38 c3
    jc short 070e9h                           ; 72 02
    mov BL, strict byte 0ffh                  ; b3 ff
    mov al, bl                                ; 88 d8
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
show_logo_:                                  ; 0xf70f2 LB 0x224
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ch                ; 83 ec 0c
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 16 a5
    mov si, ax                                ; 89 c6
    xor cl, cl                                ; 30 c9
    xor dx, dx                                ; 31 d2
    mov AL, strict byte 034h                  ; b0 34
    out strict byte 043h, AL                  ; e6 43
    mov AL, strict byte 0d3h                  ; b0 d3
    out strict byte 040h, AL                  ; e6 40
    mov AL, strict byte 048h                  ; b0 48
    out strict byte 040h, AL                  ; e6 40
    movzx ax, dl                              ; 0f b6 c2
    call 06f86h                               ; e8 68 fe
    cmp ax, 066bbh                            ; 3d bb 66
    jne near 071f6h                           ; 0f 85 d1 00
    push SS                                   ; 16
    pop ES                                    ; 07
    lea di, [bp-016h]                         ; 8d 7e ea
    mov ax, 04f03h                            ; b8 03 4f
    int 010h                                  ; cd 10
    mov word [es:di], bx                      ; 26 89 1d
    cmp ax, strict word 0004fh                ; 3d 4f 00
    jne near 071f6h                           ; 0f 85 bd 00
    mov al, dl                                ; 88 d0
    add AL, strict byte 004h                  ; 04 04
    xor ah, ah                                ; 30 e4
    call 06f70h                               ; e8 2e fe
    mov ch, al                                ; 88 c5
    mov byte [bp-00ch], al                    ; 88 46 f4
    mov al, dl                                ; 88 d0
    add AL, strict byte 005h                  ; 04 05
    xor ah, ah                                ; 30 e4
    call 06f70h                               ; e8 20 fe
    mov dh, al                                ; 88 c6
    mov byte [bp-010h], al                    ; 88 46 f0
    mov al, dl                                ; 88 d0
    add AL, strict byte 002h                  ; 04 02
    xor ah, ah                                ; 30 e4
    call 06f86h                               ; e8 28 fe
    mov bx, ax                                ; 89 c3
    mov word [bp-014h], ax                    ; 89 46 ec
    mov al, dl                                ; 88 d0
    add AL, strict byte 006h                  ; 04 06
    xor ah, ah                                ; 30 e4
    call 06f70h                               ; e8 04 fe
    mov byte [bp-012h], al                    ; 88 46 ee
    test ch, ch                               ; 84 ed
    jne short 0717dh                          ; 75 0a
    test dh, dh                               ; 84 f6
    jne short 0717dh                          ; 75 06
    test bx, bx                               ; 85 db
    je near 071f6h                            ; 0f 84 79 00
    mov bx, 00142h                            ; bb 42 01
    mov ax, 04f02h                            ; b8 02 4f
    int 010h                                  ; cd 10
    cmp byte [bp-00ch], 000h                  ; 80 7e f4 00
    je short 071aeh                           ; 74 23
    xor bx, bx                                ; 31 db
    jmp short 07195h                          ; eb 06
    inc bx                                    ; 43
    cmp bx, strict byte 00010h                ; 83 fb 10
    jnbe short 071b5h                         ; 77 20
    mov ax, bx                                ; 89 d8
    or ah, 002h                               ; 80 cc 02
    mov dx, 003b8h                            ; ba b8 03
    out DX, ax                                ; ef
    xor dx, dx                                ; 31 d2
    mov ax, strict word 00001h                ; b8 01 00
    call 06ebeh                               ; e8 18 fd
    cmp AL, strict byte 086h                  ; 3c 86
    jne short 0718fh                          ; 75 e5
    mov CL, strict byte 001h                  ; b1 01
    jmp short 071b5h                          ; eb 07
    mov ax, 00210h                            ; b8 10 02
    mov dx, 003b8h                            ; ba b8 03
    out DX, ax                                ; ef
    test cl, cl                               ; 84 c9
    jne short 071cbh                          ; 75 12
    mov ax, word [bp-014h]                    ; 8b 46 ec
    shr ax, 004h                              ; c1 e8 04
    mov dx, strict word 00001h                ; ba 01 00
    call 06ebeh                               ; e8 f9 fc
    cmp AL, strict byte 086h                  ; 3c 86
    jne short 071cbh                          ; 75 02
    mov CL, strict byte 001h                  ; b1 01
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    je short 071f6h                           ; 74 25
    test cl, cl                               ; 84 c9
    jne short 071f6h                          ; 75 21
    mov bx, strict word 00010h                ; bb 10 00
    jmp short 071dfh                          ; eb 05
    dec bx                                    ; 4b
    test bx, bx                               ; 85 db
    jbe short 071f6h                          ; 76 17
    mov ax, bx                                ; 89 d8
    or ah, 002h                               ; 80 cc 02
    mov dx, 003b8h                            ; ba b8 03
    out DX, ax                                ; ef
    xor dx, dx                                ; 31 d2
    mov ax, strict word 00001h                ; b8 01 00
    call 06ebeh                               ; e8 ce fc
    cmp AL, strict byte 086h                  ; 3c 86
    jne short 071dah                          ; 75 e6
    mov CL, strict byte 001h                  ; b1 01
    xor bx, bx                                ; 31 db
    mov dx, 00339h                            ; ba 39 03
    mov ax, si                                ; 89 f0
    call 0160eh                               ; e8 0e a4
    mov AL, strict byte 003h                  ; b0 03
    mov AH, strict byte 000h                  ; b4 00
    int 010h                                  ; cd 10
    cmp byte [bp-012h], 000h                  ; 80 7e ee 00
    je near 072f7h                            ; 0f 84 e9 00
    cmp byte [bp-00ch], 000h                  ; 80 7e f4 00
    jne short 07244h                          ; 75 30
    cmp byte [bp-010h], 000h                  ; 80 7e f0 00
    jne short 07244h                          ; 75 2a
    cmp word [bp-014h], strict byte 00000h    ; 83 7e ec 00
    jne short 07244h                          ; 75 24
    cmp byte [bp-012h], 002h                  ; 80 7e ee 02
    jne short 07231h                          ; 75 0b
    push 009cch                               ; 68 cc 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 df a6
    add sp, strict byte 00004h                ; 83 c4 04
    test cl, cl                               ; 84 c9
    jne short 07244h                          ; 75 0f
    mov dx, strict word 00001h                ; ba 01 00
    mov ax, 000c0h                            ; b8 c0 00
    call 06ebeh                               ; e8 80 fc
    cmp AL, strict byte 086h                  ; 3c 86
    jne short 07244h                          ; 75 02
    mov CL, strict byte 001h                  ; b1 01
    test cl, cl                               ; 84 c9
    je near 072f7h                            ; 0f 84 ad 00
    mov byte [bp-00eh], 000h                  ; c6 46 f2 00
    mov ax, 00100h                            ; b8 00 01
    mov cx, 01000h                            ; b9 00 10
    int 010h                                  ; cd 10
    mov ax, 00700h                            ; b8 00 07
    mov BH, strict byte 007h                  ; b7 07
    db  033h, 0c9h
    ; xor cx, cx                                ; 33 c9
    mov dx, 0184fh                            ; ba 4f 18
    int 010h                                  ; cd 10
    mov ax, 00200h                            ; b8 00 02
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    db  033h, 0d2h
    ; xor dx, dx                                ; 33 d2
    int 010h                                  ; cd 10
    push 009eeh                               ; 68 ee 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 9a a6
    add sp, strict byte 00004h                ; 83 c4 04
    call 06f9ah                               ; e8 21 fd
    push 00a32h                               ; 68 32 0a
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 8c a6
    add sp, strict byte 00004h                ; 83 c4 04
    mov dx, strict word 00001h                ; ba 01 00
    mov ax, strict word 00040h                ; b8 40 00
    call 06ebeh                               ; e8 31 fc
    mov bl, al                                ; 88 c3
    test al, al                               ; 84 c0
    je short 07284h                           ; 74 f1
    cmp AL, strict byte 030h                  ; 3c 30
    je short 072e5h                           ; 74 4e
    cmp bl, 002h                              ; 80 fb 02
    jc short 072beh                           ; 72 22
    cmp bl, 009h                              ; 80 fb 09
    jnbe short 072beh                         ; 77 1d
    movzx ax, bl                              ; 0f b6 c3
    call 070cah                               ; e8 23 fe
    cmp AL, strict byte 0ffh                  ; 3c ff
    jne short 072adh                          ; 75 02
    jmp short 07284h                          ; eb d7
    movzx bx, al                              ; 0f b6 d8
    mov dx, 00338h                            ; ba 38 03
    mov ax, si                                ; 89 f0
    call 0160eh                               ; e8 56 a3
    mov byte [bp-00eh], 002h                  ; c6 46 f2 02
    jmp short 072e5h                          ; eb 27
    cmp bl, 02eh                              ; 80 fb 2e
    je short 072d3h                           ; 74 10
    cmp bl, 026h                              ; 80 fb 26
    je short 072d9h                           ; 74 11
    cmp bl, 021h                              ; 80 fb 21
    jne short 072dfh                          ; 75 12
    mov byte [bp-00eh], 001h                  ; c6 46 f2 01
    jmp short 072e5h                          ; eb 12
    mov byte [bp-00eh], 003h                  ; c6 46 f2 03
    jmp short 072e5h                          ; eb 0c
    mov byte [bp-00eh], 004h                  ; c6 46 f2 04
    jmp short 072e5h                          ; eb 06
    cmp byte [bp-00eh], 000h                  ; 80 7e f2 00
    je short 07284h                           ; 74 9f
    movzx bx, byte [bp-00eh]                  ; 0f b6 5e f2
    mov dx, 00339h                            ; ba 39 03
    mov ax, si                                ; 89 f0
    call 0160eh                               ; e8 1d a3
    mov AL, strict byte 003h                  ; b0 03
    mov AH, strict byte 000h                  ; b4 00
    int 010h                                  ; cd 10
    mov AL, strict byte 034h                  ; b0 34
    out strict byte 043h, AL                  ; e6 43
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    out strict byte 040h, AL                  ; e6 40
    out strict byte 040h, AL                  ; e6 40
    pushad                                    ; 66 60
    push DS                                   ; 1e
    mov ds, ax                                ; 8e d8
    call 0edbfh                               ; e8 b6 7a
    pop DS                                    ; 1f
    popad                                     ; 66 61
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
delay_boot_:                                 ; 0xf7316 LB 0x67
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push dx                                   ; 52
    mov dx, ax                                ; 89 c2
    test ax, ax                               ; 85 c0
    je short 07376h                           ; 74 55
    mov AL, strict byte 034h                  ; b0 34
    out strict byte 043h, AL                  ; e6 43
    mov AL, strict byte 0d3h                  ; b0 d3
    out strict byte 040h, AL                  ; e6 40
    mov AL, strict byte 048h                  ; b0 48
    out strict byte 040h, AL                  ; e6 40
    push dx                                   ; 52
    push 00a7ch                               ; 68 7c 0a
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 d7 a5
    add sp, strict byte 00006h                ; 83 c4 06
    mov bx, dx                                ; 89 d3
    test bx, bx                               ; 85 db
    jbe short 07356h                          ; 76 17
    push bx                                   ; 53
    push 00a9ah                               ; 68 9a 0a
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 c5 a5
    add sp, strict byte 00006h                ; 83 c4 06
    xor dx, dx                                ; 31 d2
    mov ax, strict word 00040h                ; b8 40 00
    call 06ebeh                               ; e8 6b fb
    dec bx                                    ; 4b
    jmp short 0733bh                          ; eb e5
    push 009cah                               ; 68 ca 09
    push strict byte 00002h                   ; 6a 02
    call 0190dh                               ; e8 af a5
    add sp, strict byte 00004h                ; 83 c4 04
    mov AL, strict byte 034h                  ; b0 34
    out strict byte 043h, AL                  ; e6 43
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    out strict byte 040h, AL                  ; e6 40
    out strict byte 040h, AL                  ; e6 40
    pushad                                    ; 66 60
    push DS                                   ; 1e
    mov ds, ax                                ; 8e d8
    call 0edbfh                               ; e8 4c 7a
    pop DS                                    ; 1f
    popad                                     ; 66 61
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop dx                                    ; 5a
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
scsi_cmd_data_in_:                           ; 0xf737d LB 0x67
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov si, ax                                ; 89 c6
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov word [bp-008h], bx                    ; 89 5e f8
    mov dx, si                                ; 89 f2
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 0738ch                          ; 75 f7
    mov al, byte [bp-006h]                    ; 8a 46 fa
    out DX, AL                                ; ee
    xor al, al                                ; 30 c0
    out DX, AL                                ; ee
    mov al, byte [bp+004h]                    ; 8a 46 04
    out DX, AL                                ; ee
    mov al, byte [bp+00ah]                    ; 8a 46 0a
    out DX, AL                                ; ee
    mov ax, word [bp+00ah]                    ; 8b 46 0a
    shr ax, 008h                              ; c1 e8 08
    out DX, AL                                ; ee
    xor bx, bx                                ; 31 db
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp bx, ax                                ; 39 c3
    jnc short 073c5h                          ; 73 10
    mov es, cx                                ; 8e c1
    mov di, word [bp-008h]                    ; 8b 7e f8
    add di, bx                                ; 01 df
    mov al, byte [es:di]                      ; 26 8a 05
    mov dx, si                                ; 89 f2
    out DX, AL                                ; ee
    inc bx                                    ; 43
    jmp short 073adh                          ; eb e8
    mov dx, si                                ; 89 f2
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 073c5h                          ; 75 f7
    lea dx, [si+001h]                         ; 8d 54 01
    mov cx, word [bp+00ah]                    ; 8b 4e 0a
    les di, [bp+006h]                         ; c4 7e 06
    rep insb                                  ; f3 6c
    xor ax, ax                                ; 31 c0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00008h                               ; c2 08 00
scsi_cmd_data_out_:                          ; 0xf73e4 LB 0x68
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    mov di, ax                                ; 89 c7
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov word [bp-008h], bx                    ; 89 5e f8
    mov dx, di                                ; 89 fa
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 073f3h                          ; 75 f7
    mov al, byte [bp-006h]                    ; 8a 46 fa
    out DX, AL                                ; ee
    mov AL, strict byte 001h                  ; b0 01
    out DX, AL                                ; ee
    mov al, byte [bp+004h]                    ; 8a 46 04
    out DX, AL                                ; ee
    mov al, byte [bp+00ah]                    ; 8a 46 0a
    out DX, AL                                ; ee
    mov ax, word [bp+00ah]                    ; 8b 46 0a
    shr ax, 008h                              ; c1 e8 08
    out DX, AL                                ; ee
    xor bx, bx                                ; 31 db
    movzx ax, byte [bp+004h]                  ; 0f b6 46 04
    cmp bx, ax                                ; 39 c3
    jnc short 0742ch                          ; 73 10
    mov es, cx                                ; 8e c1
    mov si, word [bp-008h]                    ; 8b 76 f8
    add si, bx                                ; 01 de
    mov al, byte [es:si]                      ; 26 8a 04
    mov dx, di                                ; 89 fa
    out DX, AL                                ; ee
    inc bx                                    ; 43
    jmp short 07414h                          ; eb e8
    lea dx, [di+001h]                         ; 8d 55 01
    mov cx, word [bp+00ah]                    ; 8b 4e 0a
    les si, [bp+006h]                         ; c4 76 06
    db  0f3h, 026h, 06eh
    ; rep es outsb                              ; f3 26 6e
    mov dx, di                                ; 89 fa
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    test AL, strict byte 001h                 ; a8 01
    jne short 07438h                          ; 75 f7
    xor ax, ax                                ; 31 c0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00008h                               ; c2 08 00
@scsi_read_sectors:                          ; 0xf744c LB 0xa7
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ch                ; 83 ec 0c
    mov si, word [bp+004h]                    ; 8b 76 04
    mov es, [bp+006h]                         ; 8e 46 06
    mov bl, byte [es:si+008h]                 ; 26 8a 5c 08
    sub bl, 008h                              ; 80 eb 08
    cmp bl, 004h                              ; 80 fb 04
    jbe short 07475h                          ; 76 0f
    movzx ax, bl                              ; 0f b6 c3
    push ax                                   ; 50
    push 00a9eh                               ; 68 9e 0a
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 9b a4
    add sp, strict byte 00006h                ; 83 c4 06
    mov es, [bp+006h]                         ; 8e 46 06
    mov di, word [es:si+00ah]                 ; 26 8b 7c 0a
    mov word [bp-010h], strict word 00028h    ; c7 46 f0 28 00
    mov ax, word [es:si]                      ; 26 8b 04
    mov dx, word [es:si+002h]                 ; 26 8b 54 02
    xchg ah, al                               ; 86 c4
    xchg dh, dl                               ; 86 d6
    xchg dx, ax                               ; 92
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov word [bp-00ch], dx                    ; 89 56 f4
    mov byte [bp-00ah], 000h                  ; c6 46 f6 00
    mov ax, di                                ; 89 f8
    xchg ah, al                               ; 86 c4
    mov word [bp-009h], ax                    ; 89 46 f7
    mov byte [bp-007h], 000h                  ; c6 46 f9 00
    xor bh, bh                                ; 30 ff
    sal bx, 002h                              ; c1 e3 02
    add bx, si                                ; 01 f3
    mov ax, word [es:bx+001d8h]               ; 26 8b 87 d8 01
    mov dl, byte [es:bx+001dah]               ; 26 8a 97 da 01
    mov bx, di                                ; 89 fb
    sal bx, 009h                              ; c1 e3 09
    mov word [bp-006h], bx                    ; 89 5e fa
    push bx                                   ; 53
    db  066h, 026h, 0ffh, 074h, 004h
    ; push dword [es:si+004h]                   ; 66 26 ff 74 04
    push strict byte 0000ah                   ; 6a 0a
    xor dh, dh                                ; 30 f6
    mov cx, ss                                ; 8c d1
    lea bx, [bp-010h]                         ; 8d 5e f0
    call 0737dh                               ; e8 b0 fe
    mov ah, al                                ; 88 c4
    test al, al                               ; 84 c0
    jne short 074e7h                          ; 75 14
    mov es, [bp+006h]                         ; 8e 46 06
    mov word [es:si+014h], di                 ; 26 89 7c 14
    mov dx, word [bp-006h]                    ; 8b 56 fa
    mov word [es:si+016h], dx                 ; 26 89 54 16
    mov word [es:si+018h], strict word 00000h ; 26 c7 44 18 00 00
    movzx ax, ah                              ; 0f b6 c4
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
@scsi_write_sectors:                         ; 0xf74f3 LB 0xa7
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ch                ; 83 ec 0c
    mov si, word [bp+004h]                    ; 8b 76 04
    mov es, [bp+006h]                         ; 8e 46 06
    mov bl, byte [es:si+008h]                 ; 26 8a 5c 08
    sub bl, 008h                              ; 80 eb 08
    cmp bl, 004h                              ; 80 fb 04
    jbe short 0751ch                          ; 76 0f
    movzx ax, bl                              ; 0f b6 c3
    push ax                                   ; 50
    push 00acch                               ; 68 cc 0a
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 f4 a3
    add sp, strict byte 00006h                ; 83 c4 06
    mov es, [bp+006h]                         ; 8e 46 06
    mov di, word [es:si+00ah]                 ; 26 8b 7c 0a
    mov word [bp-010h], strict word 0002ah    ; c7 46 f0 2a 00
    mov ax, word [es:si]                      ; 26 8b 04
    mov dx, word [es:si+002h]                 ; 26 8b 54 02
    xchg ah, al                               ; 86 c4
    xchg dh, dl                               ; 86 d6
    xchg dx, ax                               ; 92
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov word [bp-00ch], dx                    ; 89 56 f4
    mov byte [bp-00ah], 000h                  ; c6 46 f6 00
    mov ax, di                                ; 89 f8
    xchg ah, al                               ; 86 c4
    mov word [bp-009h], ax                    ; 89 46 f7
    mov byte [bp-007h], 000h                  ; c6 46 f9 00
    xor bh, bh                                ; 30 ff
    sal bx, 002h                              ; c1 e3 02
    add bx, si                                ; 01 f3
    mov ax, word [es:bx+001d8h]               ; 26 8b 87 d8 01
    mov dl, byte [es:bx+001dah]               ; 26 8a 97 da 01
    mov bx, di                                ; 89 fb
    sal bx, 009h                              ; c1 e3 09
    mov word [bp-006h], bx                    ; 89 5e fa
    push bx                                   ; 53
    db  066h, 026h, 0ffh, 074h, 004h
    ; push dword [es:si+004h]                   ; 66 26 ff 74 04
    push strict byte 0000ah                   ; 6a 0a
    xor dh, dh                                ; 30 f6
    mov cx, ss                                ; 8c d1
    lea bx, [bp-010h]                         ; 8d 5e f0
    call 073e4h                               ; e8 70 fe
    mov ah, al                                ; 88 c4
    test al, al                               ; 84 c0
    jne short 0758eh                          ; 75 14
    mov es, [bp+006h]                         ; 8e 46 06
    mov word [es:si+014h], di                 ; 26 89 7c 14
    mov dx, word [bp-006h]                    ; 8b 56 fa
    mov word [es:si+016h], dx                 ; 26 89 54 16
    mov word [es:si+018h], strict word 00000h ; 26 c7 44 18 00 00
    movzx ax, ah                              ; 0f b6 c4
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
scsi_enumerate_attached_devices_:            ; 0xf759a LB 0x28b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    sub sp, 0021ah                            ; 81 ec 1a 02
    push ax                                   ; 50
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 6c a0
    mov si, 00122h                            ; be 22 01
    mov word [bp-010h], ax                    ; 89 46 f0
    mov word [bp-012h], strict word 00000h    ; c7 46 ee 00 00
    jmp near 077cah                           ; e9 0c 02
    mov es, [bp-010h]                         ; 8e 46 f0
    cmp byte [es:si+001e8h], 004h             ; 26 80 bc e8 01 04
    jnc near 0781bh                           ; 0f 83 50 02
    mov cx, strict word 0000ah                ; b9 0a 00
    xor bx, bx                                ; 31 db
    mov dx, ss                                ; 8c d2
    lea ax, [bp-024h]                         ; 8d 46 dc
    call 08e4ah                               ; e8 72 18
    mov byte [bp-024h], 025h                  ; c6 46 dc 25
    push strict byte 00008h                   ; 6a 08
    lea dx, [bp-00224h]                       ; 8d 96 dc fd
    push SS                                   ; 16
    push dx                                   ; 52
    push strict byte 0000ah                   ; 6a 0a
    movzx dx, byte [bp-012h]                  ; 0f b6 56 ee
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov ax, word [bp-00226h]                  ; 8b 86 da fd
    call 0737dh                               ; e8 87 fd
    test al, al                               ; 84 c0
    je short 07605h                           ; 74 0b
    push 00b31h                               ; 68 31 0b
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 0b a3
    add sp, strict byte 00004h                ; 83 c4 04
    movzx dx, byte [bp-00223h]                ; 0f b6 96 dd fd
    movzx di, byte [bp-00224h]                ; 0f b6 be dc fd
    sal di, 008h                              ; c1 e7 08
    xor bx, bx                                ; 31 db
    or di, dx                                 ; 09 d7
    movzx ax, byte [bp-00222h]                ; 0f b6 86 de fd
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00008h                ; b9 08 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 07620h                               ; e2 fa
    or bx, ax                                 ; 09 c3
    or di, dx                                 ; 09 d7
    movzx ax, byte [bp-00221h]                ; 0f b6 86 df fd
    or bx, ax                                 ; 09 c3
    mov word [bp-014h], bx                    ; 89 5e ec
    movzx ax, byte [bp-00220h]                ; 0f b6 86 e0 fd
    sal ax, 008h                              ; c1 e0 08
    movzx bx, byte [bp-0021fh]                ; 0f b6 9e e1 fd
    mov word [bp-01ah], strict word 00000h    ; c7 46 e6 00 00
    or bx, ax                                 ; 09 c3
    movzx ax, byte [bp-0021eh]                ; 0f b6 86 e2 fd
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00008h                ; b9 08 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 07652h                               ; e2 fa
    mov cx, word [bp-01ah]                    ; 8b 4e e6
    or cx, ax                                 ; 09 c1
    or dx, bx                                 ; 09 da
    movzx ax, byte [bp-0021dh]                ; 0f b6 86 e3 fd
    mov bx, cx                                ; 89 cb
    or bx, ax                                 ; 09 c3
    mov word [bp-016h], bx                    ; 89 5e ea
    test dx, dx                               ; 85 d2
    jne short 07675h                          ; 75 06
    cmp bx, 00200h                            ; 81 fb 00 02
    je short 07695h                           ; 74 20
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 4c a2
    push dx                                   ; 52
    push word [bp-016h]                       ; ff 76 ea
    push word [bp-012h]                       ; ff 76 ee
    push 00b6dh                               ; 68 6d 0b
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 7e a2
    add sp, strict byte 0000ah                ; 83 c4 0a
    jmp near 077c1h                           ; e9 2c 01
    cmp di, strict byte 00040h                ; 83 ff 40
    jnbe short 0769ch                         ; 77 02
    jne short 076a6h                          ; 75 0a
    mov dword [bp-00eh], strict dword 000ff003fh ; 66 c7 46 f2 3f 00 ff 00
    jmp short 076bfh                          ; eb 19
    cmp di, strict byte 00020h                ; 83 ff 20
    jnbe short 076adh                         ; 77 02
    jne short 076b7h                          ; 75 0a
    mov dword [bp-00eh], strict dword 000800020h ; 66 c7 46 f2 20 00 80 00
    jmp short 076bfh                          ; eb 08
    mov dword [bp-00eh], strict dword 000400020h ; 66 c7 46 f2 20 00 40 00
    mov bx, word [bp-00ch]                    ; 8b 5e f4
    imul bx, word [bp-00eh]                   ; 0f af 5e f2
    mov ax, word [bp-014h]                    ; 8b 46 ec
    mov dx, di                                ; 89 fa
    xor cx, cx                                ; 31 c9
    call 08de0h                               ; e8 10 17
    mov word [bp-018h], dx                    ; 89 56 e8
    mov es, [bp-010h]                         ; 8e 46 f0
    mov cl, byte [es:si+001e8h]               ; 26 8a 8c e8 01
    mov ch, cl                                ; 88 cd
    add ch, 008h                              ; 80 c5 08
    movzx dx, cl                              ; 0f b6 d1
    sal dx, 002h                              ; c1 e2 02
    mov bx, si                                ; 89 f3
    add bx, dx                                ; 01 d3
    mov dx, word [bp-00226h]                  ; 8b 96 da fd
    mov word [es:bx+001d8h], dx               ; 26 89 97 d8 01
    mov dl, byte [bp-012h]                    ; 8a 56 ee
    mov byte [es:bx+001dah], dl               ; 26 88 97 da 01
    movzx dx, ch                              ; 0f b6 d5
    imul dx, dx, strict byte 00018h           ; 6b d2 18
    mov bx, si                                ; 89 f3
    add bx, dx                                ; 01 d3
    db  066h, 026h, 0c7h, 047h, 01eh, 004h, 0ffh, 000h, 000h
    ; mov dword [es:bx+01eh], strict dword 00000ff04h ; 66 26 c7 47 1e 04 ff 00 00
    mov dx, word [bp-016h]                    ; 8b 56 ea
    mov word [es:bx+024h], dx                 ; 26 89 57 24
    mov byte [es:bx+023h], 001h               ; 26 c6 47 23 01
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    mov word [es:bx+026h], dx                 ; 26 89 57 26
    mov dx, word [bp-00eh]                    ; 8b 56 f2
    mov word [es:bx+02ah], dx                 ; 26 89 57 2a
    cmp word [bp-018h], strict byte 00000h    ; 83 7e e8 00
    jne short 07733h                          ; 75 05
    cmp ax, 00400h                            ; 3d 00 04
    jbe short 0773bh                          ; 76 08
    mov word [es:bx+028h], 00400h             ; 26 c7 47 28 00 04
    jmp short 0773fh                          ; eb 04
    mov word [es:bx+028h], ax                 ; 26 89 47 28
    movzx bx, ch                              ; 0f b6 dd
    imul bx, bx, strict byte 00018h           ; 6b db 18
    mov es, [bp-010h]                         ; 8e 46 f0
    add bx, si                                ; 01 f3
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    mov word [es:bx+02ch], dx                 ; 26 89 57 2c
    mov dx, word [bp-00eh]                    ; 8b 56 f2
    mov word [es:bx+030h], dx                 ; 26 89 57 30
    cmp word [bp-018h], strict byte 00000h    ; 83 7e e8 00
    jne short 07763h                          ; 75 05
    cmp ax, 00400h                            ; 3d 00 04
    jbe short 0776bh                          ; 76 08
    mov word [es:bx+02eh], 00400h             ; 26 c7 47 2e 00 04
    jmp short 0776fh                          ; eb 04
    mov word [es:bx+02eh], ax                 ; 26 89 47 2e
    movzx bx, ch                              ; 0f b6 dd
    imul bx, bx, strict byte 00018h           ; 6b db 18
    mov es, [bp-010h]                         ; 8e 46 f0
    add bx, si                                ; 01 f3
    mov ax, word [bp-014h]                    ; 8b 46 ec
    mov word [es:bx+032h], ax                 ; 26 89 47 32
    mov word [es:bx+034h], di                 ; 26 89 7f 34
    mov al, byte [es:si+0019eh]               ; 26 8a 84 9e 01
    mov ah, cl                                ; 88 cc
    add ah, 008h                              ; 80 c4 08
    movzx bx, al                              ; 0f b6 d8
    add bx, si                                ; 01 f3
    mov byte [es:bx+0019fh], ah               ; 26 88 a7 9f 01
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    mov byte [es:si+0019eh], al               ; 26 88 84 9e 01
    mov dx, strict word 00075h                ; ba 75 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 57 9e
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    movzx bx, al                              ; 0f b6 d8
    mov dx, strict word 00075h                ; ba 75 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 57 9e
    db  0feh, 0c1h
    ; inc cl                                    ; fe c1
    mov es, [bp-010h]                         ; 8e 46 f0
    mov byte [es:si+001e8h], cl               ; 26 88 8c e8 01
    inc word [bp-012h]                        ; ff 46 ee
    cmp word [bp-012h], strict byte 00010h    ; 83 7e ee 10
    jnl short 0781bh                          ; 7d 51
    mov byte [bp-024h], 012h                  ; c6 46 dc 12
    xor al, al                                ; 30 c0
    mov byte [bp-023h], al                    ; 88 46 dd
    mov byte [bp-022h], al                    ; 88 46 de
    mov byte [bp-021h], al                    ; 88 46 df
    mov byte [bp-020h], 005h                  ; c6 46 e0 05
    mov byte [bp-01fh], al                    ; 88 46 e1
    push strict byte 00005h                   ; 6a 05
    lea dx, [bp-00224h]                       ; 8d 96 dc fd
    push SS                                   ; 16
    push dx                                   ; 52
    push strict byte 00006h                   ; 6a 06
    movzx dx, byte [bp-012h]                  ; 0f b6 56 ee
    mov cx, ss                                ; 8c d1
    lea bx, [bp-024h]                         ; 8d 5e dc
    mov ax, word [bp-00226h]                  ; 8b 86 da fd
    call 0737dh                               ; e8 83 fb
    test al, al                               ; 84 c0
    je short 07809h                           ; 74 0b
    push 00afbh                               ; 68 fb 0a
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 07 a1
    add sp, strict byte 00004h                ; 83 c4 04
    test byte [bp-00224h], 0e0h               ; f6 86 dc fd e0
    jne short 077c1h                          ; 75 b1
    test byte [bp-00224h], 01fh               ; f6 86 dc fd 1f
    je near 075beh                            ; 0f 84 a5 fd
    jmp short 077c1h                          ; eb a6
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
_scsi_init:                                  ; 0xf7825 LB 0x66
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 eb 9d
    mov bx, 00122h                            ; bb 22 01
    mov es, ax                                ; 8e c0
    mov byte [es:bx+001e8h], 000h             ; 26 c6 87 e8 01 00
    mov AL, strict byte 055h                  ; b0 55
    mov dx, 00332h                            ; ba 32 03
    out DX, AL                                ; ee
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp AL, strict byte 055h                  ; 3c 55
    jne short 07855h                          ; 75 0c
    xor al, al                                ; 30 c0
    mov dx, 00333h                            ; ba 33 03
    out DX, AL                                ; ee
    mov ax, 00330h                            ; b8 30 03
    call 0759ah                               ; e8 45 fd
    mov AL, strict byte 055h                  ; b0 55
    mov dx, 00342h                            ; ba 42 03
    out DX, AL                                ; ee
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp AL, strict byte 055h                  ; 3c 55
    jne short 0786eh                          ; 75 0c
    xor al, al                                ; 30 c0
    mov dx, 00343h                            ; ba 43 03
    out DX, AL                                ; ee
    mov ax, 00340h                            ; b8 40 03
    call 0759ah                               ; e8 2c fd
    mov AL, strict byte 055h                  ; b0 55
    mov dx, 00352h                            ; ba 52 03
    out DX, AL                                ; ee
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp AL, strict byte 055h                  ; 3c 55
    jne short 07887h                          ; 75 0c
    xor al, al                                ; 30 c0
    mov dx, 00353h                            ; ba 53 03
    out DX, AL                                ; ee
    mov ax, 00350h                            ; b8 50 03
    call 0759ah                               ; e8 13 fd
    mov sp, bp                                ; 89 ec
    pop bp                                    ; 5d
    retn                                      ; c3
high_bits_save_:                             ; 0xf788b LB 0x17
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    mov bx, ax                                ; 89 c3
    shr eax, 010h                             ; 66 c1 e8 10
    mov es, dx                                ; 8e c2
    mov word [es:bx+00268h], ax               ; 26 89 87 68 02
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
high_bits_restore_:                          ; 0xf78a2 LB 0x17
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    mov bx, ax                                ; 89 c3
    mov es, dx                                ; 8e c2
    mov ax, word [es:bx+00268h]               ; 26 8b 87 68 02
    sal eax, 010h                             ; 66 c1 e0 10
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_ctrl_set_bits_:                         ; 0xf78b9 LB 0x43
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov si, ax                                ; 89 c6
    mov ax, dx                                ; 89 d0
    mov word [bp-006h], bx                    ; 89 5e fa
    mov di, cx                                ; 89 cf
    xor cx, cx                                ; 31 c9
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea bx, [si+004h]                         ; 8d 5c 04
    mov dx, bx                                ; 89 da
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    or ax, word [bp-006h]                     ; 0b 46 fa
    mov cx, dx                                ; 89 d1
    or cx, di                                 ; 09 f9
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_ctrl_clear_bits_:                       ; 0xf78fc LB 0x47
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    mov si, ax                                ; 89 c6
    mov ax, dx                                ; 89 d0
    mov di, bx                                ; 89 df
    mov word [bp-006h], cx                    ; 89 4e fa
    xor cx, cx                                ; 31 c9
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea bx, [si+004h]                         ; 8d 5c 04
    mov dx, bx                                ; 89 da
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    not di                                    ; f7 d7
    mov cx, word [bp-006h]                    ; 8b 4e fa
    not cx                                    ; f7 d1
    and ax, di                                ; 21 f8
    and cx, dx                                ; 21 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_ctrl_is_bit_set_:                       ; 0xf7943 LB 0x39
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    mov si, ax                                ; 89 c6
    mov ax, dx                                ; 89 d0
    mov di, cx                                ; 89 cf
    xor cx, cx                                ; 31 c9
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea dx, [si+004h]                         ; 8d 54 04
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    test dx, di                               ; 85 fa
    jne short 0796fh                          ; 75 04
    test ax, bx                               ; 85 d8
    je short 07973h                           ; 74 04
    mov AL, strict byte 001h                  ; b0 01
    jmp short 07975h                          ; eb 02
    xor al, al                                ; 30 c0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_ctrl_extract_bits_:                     ; 0xf797c LB 0x1b
    push si                                   ; 56
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov si, bx                                ; 89 de
    and ax, bx                                ; 21 d8
    and dx, cx                                ; 21 ca
    movzx cx, byte [bp+006h]                  ; 0f b6 4e 06
    jcxz 07992h                               ; e3 06
    shr dx, 1                                 ; d1 ea
    rcr ax, 1                                 ; d1 d8
    loop 0798ch                               ; e2 fa
    pop bp                                    ; 5d
    pop si                                    ; 5e
    retn 00002h                               ; c2 02 00
ahci_addr_to_phys_:                          ; 0xf7997 LB 0x1e
    push bx                                   ; 53
    push cx                                   ; 51
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, ax                                ; 89 c3
    mov ax, dx                                ; 89 d0
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00004h                ; b9 04 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 079a5h                               ; e2 fa
    xor cx, cx                                ; 31 c9
    add ax, bx                                ; 01 d8
    adc dx, cx                                ; 11 ca
    pop bp                                    ; 5d
    pop cx                                    ; 59
    pop bx                                    ; 5b
    retn                                      ; c3
ahci_port_cmd_sync_:                         ; 0xf79b5 LB 0xd5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov si, ax                                ; 89 c6
    mov word [bp-00ah], dx                    ; 89 56 f6
    mov es, dx                                ; 8e c2
    mov al, byte [es:si+00262h]               ; 26 8a 84 62 02
    mov byte [bp-008h], al                    ; 88 46 f8
    mov di, word [es:si+00260h]               ; 26 8b bc 60 02
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 07a82h                            ; 0f 84 aa 00
    movzx cx, byte [es:si+00263h]             ; 26 0f b6 8c 63 02
    xor dx, dx                                ; 31 d2
    or dl, 080h                               ; 80 ca 80
    movzx ax, bl                              ; 0f b6 c3
    or dx, ax                                 ; 09 c2
    mov word [es:si], dx                      ; 26 89 14
    mov word [es:si+002h], cx                 ; 26 89 4c 02
    db  066h, 026h, 0c7h, 044h, 004h, 000h, 000h, 000h, 000h
    ; mov dword [es:si+004h], strict dword 000000000h ; 66 26 c7 44 04 00 00 00 00
    lea ax, [si+00080h]                       ; 8d 84 80 00
    mov dx, es                                ; 8c c2
    call 07997h                               ; e8 96 ff
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov word [es:si+008h], ax                 ; 26 89 44 08
    mov word [es:si+00ah], dx                 ; 26 89 54 0a
    movzx si, byte [bp-008h]                  ; 0f b6 76 f8
    sal si, 007h                              ; c1 e6 07
    lea dx, [si+00118h]                       ; 8d 94 18 01
    mov bx, strict word 00011h                ; bb 11 00
    xor cx, cx                                ; 31 c9
    mov ax, di                                ; 89 f8
    call 078b9h                               ; e8 98 fe
    lea ax, [si+00138h]                       ; 8d 84 38 01
    cwd                                       ; 99
    mov cx, dx                                ; 89 d1
    mov dx, di                                ; 89 fa
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea dx, [di+004h]                         ; 8d 55 04
    mov ax, strict word 00001h                ; b8 01 00
    xor cx, cx                                ; 31 c9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    sal ax, 007h                              ; c1 e0 07
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov si, ax                                ; 89 c6
    add si, 00110h                            ; 81 c6 10 01
    mov bx, strict word 00001h                ; bb 01 00
    mov cx, 04000h                            ; b9 00 40
    mov dx, si                                ; 89 f2
    mov ax, di                                ; 89 f8
    call 07943h                               ; e8 e2 fe
    test al, al                               ; 84 c0
    je short 07a44h                           ; 74 df
    mov bx, strict word 00001h                ; bb 01 00
    xor cx, cx                                ; 31 c9
    mov dx, si                                ; 89 f2
    mov ax, di                                ; 89 f8
    call 078b9h                               ; e8 48 fe
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    add dx, 00118h                            ; 81 c2 18 01
    mov bx, strict word 00001h                ; bb 01 00
    xor cx, cx                                ; 31 c9
    mov ax, di                                ; 89 f8
    call 078fch                               ; e8 7a fe
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_cmd_data_:                              ; 0xf7a8a LB 0x1ca
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00010h                ; 83 ec 10
    mov di, ax                                ; 89 c7
    mov word [bp-012h], dx                    ; 89 56 ee
    mov byte [bp-008h], bl                    ; 88 5e f8
    xor si, si                                ; 31 f6
    mov es, dx                                ; 8e c2
    mov ax, word [es:di+001eeh]               ; 26 8b 85 ee 01
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov word [bp-00eh], si                    ; 89 76 f2
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov ax, word [es:di+00ah]                 ; 26 8b 45 0a
    mov word [bp-010h], ax                    ; 89 46 f0
    mov ax, word [es:di+00ch]                 ; 26 8b 45 0c
    mov word [bp-016h], ax                    ; 89 46 ea
    mov cx, strict word 00040h                ; b9 40 00
    xor bx, bx                                ; 31 db
    mov ax, 00080h                            ; b8 80 00
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 08e4ah                               ; e8 81 13
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov word [es:si+00080h], 08027h           ; 26 c7 84 80 00 27 80
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov byte [es:si+00082h], al               ; 26 88 84 82 00
    mov byte [es:si+00083h], 000h             ; 26 c6 84 83 00 00
    mov es, [bp-012h]                         ; 8e 46 ee
    mov al, byte [es:di]                      ; 26 8a 05
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov byte [es:si+00084h], al               ; 26 88 84 84 00
    mov es, [bp-012h]                         ; 8e 46 ee
    mov ax, word [es:di]                      ; 26 8b 05
    mov bx, word [es:di+002h]                 ; 26 8b 5d 02
    mov cx, strict word 00008h                ; b9 08 00
    shr bx, 1                                 ; d1 eb
    rcr ax, 1                                 ; d1 d8
    loop 07afch                               ; e2 fa
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov byte [es:si+00085h], al               ; 26 88 84 85 00
    mov es, [bp-012h]                         ; 8e 46 ee
    mov ax, word [es:di+002h]                 ; 26 8b 45 02
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov byte [es:si+00086h], al               ; 26 88 84 86 00
    mov byte [es:si+00087h], 040h             ; 26 c6 84 87 00 40
    mov es, [bp-012h]                         ; 8e 46 ee
    mov ax, word [es:di+002h]                 ; 26 8b 45 02
    shr ax, 008h                              ; c1 e8 08
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov byte [es:si+00088h], al               ; 26 88 84 88 00
    mov word [es:si+00089h], strict word 00000h ; 26 c7 84 89 00 00 00
    mov byte [es:si+0008bh], 000h             ; 26 c6 84 8b 00 00
    mov al, byte [bp-010h]                    ; 8a 46 f0
    mov byte [es:si+0008ch], al               ; 26 88 84 8c 00
    mov ax, word [bp-010h]                    ; 8b 46 f0
    shr ax, 008h                              ; c1 e8 08
    mov byte [es:si+0008dh], al               ; 26 88 84 8d 00
    mov word [es:si+00276h], strict word 00010h ; 26 c7 84 76 02 10 00
    mov ax, word [bp-010h]                    ; 8b 46 f0
    xor dx, dx                                ; 31 d2
    mov bx, word [bp-016h]                    ; 8b 5e ea
    xor cx, cx                                ; 31 c9
    call 08e19h                               ; e8 b4 12
    push dx                                   ; 52
    push ax                                   ; 50
    mov es, [bp-012h]                         ; 8e 46 ee
    mov bx, word [es:di+004h]                 ; 26 8b 5d 04
    mov cx, word [es:di+006h]                 ; 26 8b 4d 06
    mov ax, 0026ah                            ; b8 6a 02
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 08d37h                               ; e8 bc 11
    mov es, [bp-00ah]                         ; 8e 46 f6
    movzx ax, byte [es:si+00263h]             ; 26 0f b6 84 63 02
    mov dx, word [es:si+0027eh]               ; 26 8b 94 7e 02
    add dx, strict byte 0ffffh                ; 83 c2 ff
    mov bx, word [es:si+00280h]               ; 26 8b 9c 80 02
    adc bx, strict byte 0ffffh                ; 83 d3 ff
    mov word [bp-014h], bx                    ; 89 5e ec
    mov bx, ax                                ; 89 c3
    sal bx, 004h                              ; c1 e3 04
    mov word [es:bx+0010ch], dx               ; 26 89 97 0c 01
    mov dx, word [bp-014h]                    ; 8b 56 ec
    mov word [es:bx+0010eh], dx               ; 26 89 97 0e 01
    mov cx, word [es:si+0027ah]               ; 26 8b 8c 7a 02
    mov dx, word [es:si+0027ch]               ; 26 8b 94 7c 02
    mov word [es:bx+00100h], cx               ; 26 89 8f 00 01
    mov word [es:bx+00102h], dx               ; 26 89 97 02 01
    inc ax                                    ; 40
    mov es, [bp-012h]                         ; 8e 46 ee
    cmp word [es:di+01ch], strict byte 00000h ; 26 83 7d 1c 00
    je short 07bf4h                           ; 74 2c
    mov dx, word [es:di+01ch]                 ; 26 8b 55 1c
    dec dx                                    ; 4a
    mov di, ax                                ; 89 c7
    sal di, 004h                              ; c1 e7 04
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov word [es:di+0010ch], dx               ; 26 89 95 0c 01
    mov word [es:di+0010eh], si               ; 26 89 b5 0e 01
    mov dx, word [es:si+00264h]               ; 26 8b 94 64 02
    mov bx, word [es:si+00266h]               ; 26 8b 9c 66 02
    mov word [es:di+00100h], dx               ; 26 89 95 00 01
    mov word [es:di+00102h], bx               ; 26 89 9d 02 01
    inc ax                                    ; 40
    les bx, [bp-00eh]                         ; c4 5e f2
    mov byte [es:bx+00263h], al               ; 26 88 87 63 02
    xor ax, ax                                ; 31 c0
    les bx, [bp-00eh]                         ; c4 5e f2
    movzx dx, byte [es:bx+00263h]             ; 26 0f b6 97 63 02
    cmp ax, dx                                ; 39 d0
    jnc short 07c0eh                          ; 73 03
    inc ax                                    ; 40
    jmp short 07bfeh                          ; eb f0
    mov al, byte [bp-008h]                    ; 8a 46 f8
    cmp AL, strict byte 035h                  ; 3c 35
    jne short 07c1bh                          ; 75 06
    mov byte [bp-008h], 040h                  ; c6 46 f8 40
    jmp short 07c2fh                          ; eb 14
    cmp AL, strict byte 0a0h                  ; 3c a0
    jne short 07c2bh                          ; 75 0c
    or byte [bp-008h], 020h                   ; 80 4e f8 20
    or byte [es:bx+00083h], 001h              ; 26 80 8f 83 00 01
    jmp short 07c2fh                          ; eb 04
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    or byte [bp-008h], 005h                   ; 80 4e f8 05
    movzx bx, byte [bp-008h]                  ; 0f b6 5e f8
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    call 079b5h                               ; e8 75 fd
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    add ax, 0026ah                            ; 05 6a 02
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    call 08db0h                               ; e8 64 11
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_port_deinit_current_:                   ; 0xf7c54 LB 0x144
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov di, ax                                ; 89 c7
    mov word [bp-00ch], dx                    ; 89 56 f4
    mov es, dx                                ; 8e c2
    mov si, word [es:di+00260h]               ; 26 8b b5 60 02
    mov al, byte [es:di+00262h]               ; 26 8a 85 62 02
    mov byte [bp-00ah], al                    ; 88 46 f6
    cmp AL, strict byte 0ffh                  ; 3c ff
    je near 07d8fh                            ; 0f 84 17 01
    movzx dx, al                              ; 0f b6 d0
    sal dx, 007h                              ; c1 e2 07
    add dx, 00118h                            ; 81 c2 18 01
    mov bx, strict word 00011h                ; bb 11 00
    xor cx, cx                                ; 31 c9
    mov ax, si                                ; 89 f0
    call 078fch                               ; e8 70 fc
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    sal ax, 007h                              ; c1 e0 07
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov dx, ax                                ; 89 c2
    add dx, 00118h                            ; 81 c2 18 01
    mov bx, 0c011h                            ; bb 11 c0
    xor cx, cx                                ; 31 c9
    mov ax, si                                ; 89 f0
    call 07943h                               ; e8 9d fc
    cmp AL, strict byte 001h                  ; 3c 01
    je short 07c8ch                           ; 74 e2
    mov cx, strict word 00020h                ; b9 20 00
    xor bx, bx                                ; 31 db
    mov ax, di                                ; 89 f8
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    call 08e4ah                               ; e8 93 11
    lea ax, [di+00080h]                       ; 8d 85 80 00
    mov cx, strict word 00040h                ; b9 40 00
    xor bx, bx                                ; 31 db
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    call 08e4ah                               ; e8 84 11
    lea ax, [di+00200h]                       ; 8d 85 00 02
    mov cx, strict word 00060h                ; b9 60 00
    xor bx, bx                                ; 31 db
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    call 08e4ah                               ; e8 75 11
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    add ax, 00108h                            ; 05 08 01
    cwd                                       ; 99
    mov cx, dx                                ; 89 d1
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea bx, [si+004h]                         ; 8d 5c 04
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    add ax, 0010ch                            ; 05 0c 01
    cwd                                       ; 99
    mov cx, dx                                ; 89 d1
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    db  0feh, 0c4h
    ; inc ah                                    ; fe c4
    cwd                                       ; 99
    mov cx, dx                                ; 89 d1
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    add ax, 00104h                            ; 05 04 01
    cwd                                       ; 99
    mov cx, dx                                ; 89 d1
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    add ax, 00114h                            ; 05 14 01
    cwd                                       ; 99
    mov cx, dx                                ; 89 d1
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-00ch]                         ; 8e 46 f4
    mov byte [es:di+00262h], 0ffh             ; 26 c6 85 62 02 ff
    lea sp, [bp-008h]                         ; 8d 66 f8
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_port_init_:                             ; 0xf7d98 LB 0x206
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov si, ax                                ; 89 c6
    mov word [bp-00ah], dx                    ; 89 56 f6
    mov byte [bp-008h], bl                    ; 88 5e f8
    call 07c54h                               ; e8 a8 fe
    movzx dx, bl                              ; 0f b6 d3
    sal dx, 007h                              ; c1 e2 07
    add dx, 00118h                            ; 81 c2 18 01
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov ax, word [es:si+00260h]               ; 26 8b 84 60 02
    mov bx, strict word 00011h                ; bb 11 00
    xor cx, cx                                ; 31 c9
    call 078fch                               ; e8 36 fb
    movzx di, byte [bp-008h]                  ; 0f b6 7e f8
    sal di, 007h                              ; c1 e7 07
    lea dx, [di+00118h]                       ; 8d 95 18 01
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov ax, word [es:si+00260h]               ; 26 8b 84 60 02
    mov bx, 0c011h                            ; bb 11 c0
    xor cx, cx                                ; 31 c9
    call 07943h                               ; e8 62 fb
    cmp AL, strict byte 001h                  ; 3c 01
    je short 07dc6h                           ; 74 e1
    mov cx, strict word 00020h                ; b9 20 00
    xor bx, bx                                ; 31 db
    mov ax, si                                ; 89 f0
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 08e4ah                               ; e8 58 10
    lea ax, [si+00080h]                       ; 8d 84 80 00
    mov cx, strict word 00040h                ; b9 40 00
    xor bx, bx                                ; 31 db
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 08e4ah                               ; e8 49 10
    mov ax, si                                ; 89 f0
    add ah, 002h                              ; 80 c4 02
    mov word [bp-00ch], ax                    ; 89 46 f4
    mov cx, strict word 00060h                ; b9 60 00
    xor bx, bx                                ; 31 db
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 08e4ah                               ; e8 36 10
    lea ax, [di+00108h]                       ; 8d 85 08 01
    cwd                                       ; 99
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 07997h                               ; e8 60 fb
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    add bx, strict byte 00004h                ; 83 c3 04
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea ax, [di+0010ch]                       ; 8d 85 0c 01
    cwd                                       ; 99
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov dx, word [es:si+00260h]               ; 26 8b 94 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea ax, [di+00100h]                       ; 8d 85 00 01
    cwd                                       ; 99
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov ax, si                                ; 89 f0
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 07997h                               ; e8 f4 fa
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    add bx, strict byte 00004h                ; 83 c3 04
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea ax, [di+00104h]                       ; 8d 85 04 01
    cwd                                       ; 99
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov dx, word [es:si+00260h]               ; 26 8b 94 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea ax, [di+00114h]                       ; 8d 85 14 01
    cwd                                       ; 99
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov dx, word [es:si+00260h]               ; 26 8b 94 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea ax, [di+00110h]                       ; 8d 85 10 01
    cwd                                       ; 99
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov dx, word [es:si+00260h]               ; 26 8b 94 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    mov ax, strict word 0ffffh                ; b8 ff ff
    mov cx, ax                                ; 89 c1
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea ax, [di+00130h]                       ; 8d 85 30 01
    cwd                                       ; 99
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov bx, word [es:si+00260h]               ; 26 8b 9c 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov dx, word [es:si+00260h]               ; 26 8b 94 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    mov ax, strict word 0ffffh                ; b8 ff ff
    mov cx, ax                                ; 89 c1
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov byte [es:si+00262h], al               ; 26 88 84 62 02
    mov byte [es:si+00263h], 000h             ; 26 c6 84 63 02 00
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
@ahci_read_sectors:                          ; 0xf7f9e LB 0x94
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    les di, [bp+004h]                         ; c4 7e 04
    movzx di, byte [es:di+008h]               ; 26 0f b6 7d 08
    sub di, strict byte 0000ch                ; 83 ef 0c
    cmp di, strict byte 00004h                ; 83 ff 04
    jbe short 07fc2h                          ; 76 0f
    push di                                   ; 57
    push 00b9ch                               ; 68 9c 0b
    push 00baeh                               ; 68 ae 0b
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 4e 99
    add sp, strict byte 00008h                ; 83 c4 08
    les bx, [bp+004h]                         ; c4 5e 04
    mov dx, word [es:bx+001eeh]               ; 26 8b 97 ee 01
    xor ax, ax                                ; 31 c0
    call 0788bh                               ; e8 bc f8
    mov es, [bp+006h]                         ; 8e 46 06
    add di, bx                                ; 01 df
    movzx bx, byte [es:di+001e9h]             ; 26 0f b6 9d e9 01
    mov di, word [bp+004h]                    ; 8b 7e 04
    mov dx, word [es:di+001eeh]               ; 26 8b 95 ee 01
    xor ax, ax                                ; 31 c0
    call 07d98h                               ; e8 b1 fd
    mov bx, strict word 00025h                ; bb 25 00
    mov ax, di                                ; 89 f8
    mov dx, word [bp+006h]                    ; 8b 56 06
    call 07a8ah                               ; e8 98 fa
    mov es, [bp+006h]                         ; 8e 46 06
    mov bx, di                                ; 89 fb
    mov ax, word [es:bx+00ah]                 ; 26 8b 47 0a
    mov word [es:bx+014h], ax                 ; 26 89 47 14
    mov cx, ax                                ; 89 c1
    sal cx, 009h                              ; c1 e1 09
    shr cx, 1                                 ; d1 e9
    mov di, word [es:di+004h]                 ; 26 8b 7d 04
    mov ax, word [es:bx+006h]                 ; 26 8b 47 06
    mov si, di                                ; 89 fe
    mov dx, ax                                ; 89 c2
    mov es, ax                                ; 8e c0
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsw                                 ; f3 a5
    pop DS                                    ; 1f
    mov es, [bp+006h]                         ; 8e 46 06
    mov dx, word [es:bx+001eeh]               ; 26 8b 97 ee 01
    xor ax, ax                                ; 31 c0
    call 078a2h                               ; e8 7b f8
    xor ax, ax                                ; 31 c0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
@ahci_write_sectors:                         ; 0xf8032 LB 0x70
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    mov si, word [bp+004h]                    ; 8b 76 04
    mov cx, word [bp+006h]                    ; 8b 4e 06
    mov es, cx                                ; 8e c1
    movzx bx, byte [es:si+008h]               ; 26 0f b6 5c 08
    sub bx, strict byte 0000ch                ; 83 eb 0c
    cmp bx, strict byte 00004h                ; 83 fb 04
    jbe short 0805ah                          ; 76 0f
    push bx                                   ; 53
    push 00bcdh                               ; 68 cd 0b
    push 00baeh                               ; 68 ae 0b
    push strict byte 00007h                   ; 6a 07
    call 0190dh                               ; e8 b6 98
    add sp, strict byte 00008h                ; 83 c4 08
    mov es, cx                                ; 8e c1
    mov dx, word [es:si+001eeh]               ; 26 8b 94 ee 01
    xor ax, ax                                ; 31 c0
    call 0788bh                               ; e8 25 f8
    mov es, cx                                ; 8e c1
    add bx, si                                ; 01 f3
    movzx bx, byte [es:bx+001e9h]             ; 26 0f b6 9f e9 01
    mov dx, word [es:si+001eeh]               ; 26 8b 94 ee 01
    xor ax, ax                                ; 31 c0
    call 07d98h                               ; e8 1e fd
    mov bx, strict word 00035h                ; bb 35 00
    mov ax, si                                ; 89 f0
    mov dx, cx                                ; 89 ca
    call 07a8ah                               ; e8 06 fa
    mov es, cx                                ; 8e c1
    mov dx, word [es:si+00ah]                 ; 26 8b 54 0a
    mov word [es:si+014h], dx                 ; 26 89 54 14
    mov dx, word [es:si+001eeh]               ; 26 8b 94 ee 01
    xor ax, ax                                ; 31 c0
    call 078a2h                               ; e8 0a f8
    xor ax, ax                                ; 31 c0
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
ahci_cmd_packet_:                            ; 0xf80a2 LB 0x173
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000eh                ; 83 ec 0e
    push ax                                   ; 50
    mov byte [bp-006h], dl                    ; 88 56 fa
    mov word [bp-012h], bx                    ; 89 5e ee
    mov word [bp-010h], cx                    ; 89 4e f0
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 5f 95
    mov si, 00122h                            ; be 22 01
    mov word [bp-008h], ax                    ; 89 46 f8
    cmp byte [bp+00ah], 002h                  ; 80 7e 0a 02
    jne short 080e8h                          ; 75 1f
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 f8 97
    push 00be0h                               ; 68 e0 0b
    push 00bf0h                               ; 68 f0 0b
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 2e 98
    add sp, strict byte 00006h                ; 83 c4 06
    mov ax, strict word 00001h                ; b8 01 00
    jmp near 0820ch                           ; e9 24 01
    test byte [bp+004h], 001h                 ; f6 46 04 01
    jne short 080e2h                          ; 75 f4
    mov ax, word [bp+006h]                    ; 8b 46 06
    mov dx, word [bp+008h]                    ; 8b 56 08
    mov cx, strict word 00008h                ; b9 08 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 080f7h                               ; e2 fa
    mov es, [bp-008h]                         ; 8e 46 f8
    mov word [es:si], ax                      ; 26 89 04
    mov word [es:si+002h], dx                 ; 26 89 54 02
    mov ax, word [bp+00ch]                    ; 8b 46 0c
    mov word [es:si+004h], ax                 ; 26 89 44 04
    mov ax, word [bp+00eh]                    ; 8b 46 0e
    mov word [es:si+006h], ax                 ; 26 89 44 06
    mov bx, word [es:si+00ch]                 ; 26 8b 5c 0c
    mov ax, word [bp+006h]                    ; 8b 46 06
    mov dx, word [bp+008h]                    ; 8b 56 08
    xor cx, cx                                ; 31 c9
    call 08de0h                               ; e8 bc 0c
    mov word [es:si+00ah], ax                 ; 26 89 44 0a
    xor di, di                                ; 31 ff
    mov ax, word [es:si+001eeh]               ; 26 8b 84 ee 01
    mov word [bp-00ah], ax                    ; 89 46 f6
    mov word [bp-00eh], di                    ; 89 7e f2
    mov word [bp-00ch], ax                    ; 89 46 f4
    sub word [bp-014h], strict byte 0000ch    ; 83 6e ec 0c
    xor ax, ax                                ; 31 c0
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 0788bh                               ; e8 47 f7
    mov es, [bp-008h]                         ; 8e 46 f8
    mov bx, word [bp-014h]                    ; 8b 5e ec
    add bx, si                                ; 01 f3
    movzx bx, byte [es:bx+001e9h]             ; 26 0f b6 9f e9 01
    mov dx, word [es:si+001eeh]               ; 26 8b 94 ee 01
    xor ax, ax                                ; 31 c0
    call 07d98h                               ; e8 3c fc
    movzx ax, byte [bp-006h]                  ; 0f b6 46 fa
    push ax                                   ; 50
    mov bx, word [bp-012h]                    ; 8b 5e ee
    mov cx, word [bp-010h]                    ; 8b 4e f0
    mov ax, 000c0h                            ; b8 c0 00
    mov dx, word [bp-00ah]                    ; 8b 56 f6
    call 08e57h                               ; e8 e7 0c
    mov es, [bp-008h]                         ; 8e 46 f8
    mov word [es:si+014h], di                 ; 26 89 7c 14
    mov word [es:si+016h], di                 ; 26 89 7c 16
    mov word [es:si+018h], di                 ; 26 89 7c 18
    mov ax, word [es:si+01ah]                 ; 26 8b 44 1a
    test ax, ax                               ; 85 c0
    je short 081aeh                           ; 74 27
    dec ax                                    ; 48
    mov es, [bp-00ah]                         ; 8e 46 f6
    mov word [es:di+0010ch], ax               ; 26 89 85 0c 01
    mov word [es:di+0010eh], di               ; 26 89 bd 0e 01
    mov ax, word [es:di+00264h]               ; 26 8b 85 64 02
    mov dx, word [es:di+00266h]               ; 26 8b 95 66 02
    mov word [es:di+00100h], ax               ; 26 89 85 00 01
    mov word [es:di+00102h], dx               ; 26 89 95 02 01
    inc byte [es:di+00263h]                   ; 26 fe 85 63 02
    mov bx, 000a0h                            ; bb a0 00
    mov ax, si                                ; 89 f0
    mov dx, word [bp-008h]                    ; 8b 56 f8
    call 07a8ah                               ; e8 d1 f8
    les bx, [bp-00eh]                         ; c4 5e f2
    mov ax, word [es:bx+004h]                 ; 26 8b 47 04
    mov dx, word [es:bx+006h]                 ; 26 8b 57 06
    mov es, [bp-008h]                         ; 8e 46 f8
    mov word [es:si+016h], ax                 ; 26 89 44 16
    mov word [es:si+018h], dx                 ; 26 89 54 18
    mov bx, word [es:si+016h]                 ; 26 8b 5c 16
    mov cx, dx                                ; 89 d1
    shr cx, 1                                 ; d1 e9
    rcr bx, 1                                 ; d1 db
    mov di, word [es:si+004h]                 ; 26 8b 7c 04
    mov ax, word [es:si+006h]                 ; 26 8b 44 06
    mov cx, bx                                ; 89 d9
    mov si, di                                ; 89 fe
    mov dx, ax                                ; 89 c2
    mov es, ax                                ; 8e c0
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsw                                 ; f3 a5
    pop DS                                    ; 1f
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    mov dx, word [bp-00ch]                    ; 8b 56 f4
    call 078a2h                               ; e8 aa f6
    les bx, [bp-00eh]                         ; c4 5e f2
    mov ax, word [es:bx+006h]                 ; 26 8b 47 06
    or ax, word [es:bx+004h]                  ; 26 0b 47 04
    jne short 0820ah                          ; 75 05
    mov ax, strict word 00004h                ; b8 04 00
    jmp short 0820ch                          ; eb 02
    xor ax, ax                                ; 31 c0
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 0000ch                               ; c2 0c 00
ahci_port_detect_device_:                    ; 0xf8215 LB 0x3c9
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    sub sp, 00220h                            ; 81 ec 20 02
    mov di, ax                                ; 89 c7
    mov word [bp-010h], dx                    ; 89 56 f0
    mov byte [bp-008h], bl                    ; 88 5e f8
    movzx cx, bl                              ; 0f b6 cb
    mov bx, cx                                ; 89 cb
    call 07d98h                               ; e8 69 fb
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 e4 93
    mov word [bp-024h], ax                    ; 89 46 dc
    mov si, 00122h                            ; be 22 01
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov word [bp-014h], si                    ; 89 76 ec
    mov word [bp-012h], ax                    ; 89 46 ee
    sal cx, 007h                              ; c1 e1 07
    mov word [bp-018h], cx                    ; 89 4e e8
    mov ax, cx                                ; 89 c8
    add ax, 0012ch                            ; 05 2c 01
    cwd                                       ; 99
    mov word [bp-026h], ax                    ; 89 46 da
    mov bx, dx                                ; 89 d3
    mov es, [bp-010h]                         ; 8e 46 f0
    mov dx, word [es:di+00260h]               ; 26 8b 95 60 02
    mov cx, bx                                ; 89 d9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-010h]                         ; 8e 46 f0
    mov dx, word [es:di+00260h]               ; 26 8b 95 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    mov ax, strict word 00001h                ; b8 01 00
    xor cx, cx                                ; 31 c9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-010h]                         ; 8e 46 f0
    mov dx, word [es:di+00260h]               ; 26 8b 95 60 02
    mov ax, word [bp-026h]                    ; 8b 46 da
    mov cx, bx                                ; 89 d9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-010h]                         ; 8e 46 f0
    mov dx, word [es:di+00260h]               ; 26 8b 95 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov ax, word [bp-018h]                    ; 8b 46 e8
    add ax, 00128h                            ; 05 28 01
    cwd                                       ; 99
    mov es, [bp-010h]                         ; 8e 46 f0
    mov bx, word [es:di+00260h]               ; 26 8b 9d 60 02
    mov cx, dx                                ; 89 d1
    mov dx, bx                                ; 89 da
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-010h]                         ; 8e 46 f0
    mov dx, word [es:di+00260h]               ; 26 8b 95 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    push strict byte 00000h                   ; 6a 00
    mov bx, strict word 0000fh                ; bb 0f 00
    xor cx, cx                                ; 31 c9
    call 0797ch                               ; e8 90 f6
    cmp ax, strict word 00003h                ; 3d 03 00
    jne near 085d6h                           ; 0f 85 e3 02
    mov es, [bp-00eh]                         ; 8e 46 f2
    mov al, byte [es:si+001edh]               ; 26 8a 84 ed 01
    mov byte [bp-00ah], al                    ; 88 46 f6
    cmp AL, strict byte 004h                  ; 3c 04
    jnc near 085d6h                           ; 0f 83 d2 02
    mov dx, word [bp-018h]                    ; 8b 56 e8
    add dx, 00118h                            ; 81 c2 18 01
    mov es, [bp-010h]                         ; 8e 46 f0
    mov ax, word [es:di+00260h]               ; 26 8b 85 60 02
    mov bx, strict word 00010h                ; bb 10 00
    xor cx, cx                                ; 31 c9
    call 078b9h                               ; e8 9e f5
    mov ax, word [bp-018h]                    ; 8b 46 e8
    add ax, 00124h                            ; 05 24 01
    cwd                                       ; 99
    mov cx, dx                                ; 89 d1
    mov es, [bp-010h]                         ; 8e 46 f0
    mov dx, word [es:di+00260h]               ; 26 8b 95 60 02
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov es, [bp-010h]                         ; 8e 46 f0
    mov dx, word [es:di+00260h]               ; 26 8b 95 60 02
    add dx, strict byte 00004h                ; 83 c2 04
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    mov bx, ax                                ; 89 c3
    mov ax, dx                                ; 89 d0
    mov cl, byte [bp-00ah]                    ; 8a 4e f6
    add cl, 00ch                              ; 80 c1 0c
    test dx, dx                               ; 85 d2
    jne near 08530h                           ; 0f 85 d7 01
    cmp bx, 00101h                            ; 81 fb 01 01
    jne near 08530h                           ; 0f 85 cf 01
    mov es, [bp-00eh]                         ; 8e 46 f2
    db  066h, 026h, 0c7h, 004h, 000h, 000h, 000h, 000h
    ; mov dword [es:si], strict dword 000000000h ; 66 26 c7 04 00 00 00 00
    lea dx, [bp-00226h]                       ; 8d 96 da fd
    mov word [es:si+004h], dx                 ; 26 89 54 04
    mov [es:si+006h], ss                      ; 26 8c 54 06
    db  066h, 026h, 0c7h, 044h, 00ah, 001h, 000h, 000h, 002h
    ; mov dword [es:si+00ah], strict dword 002000001h ; 66 26 c7 44 0a 01 00 00 02
    mov bx, 000ech                            ; bb ec 00
    mov ax, si                                ; 89 f0
    mov dx, word [bp-024h]                    ; 8b 56 dc
    call 07a8ah                               ; e8 fe f6
    mov byte [bp-00ch], cl                    ; 88 4e f4
    test byte [bp-00226h], 080h               ; f6 86 da fd 80
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    mov dx, word [bp-00224h]                  ; 8b 96 dc fd
    mov word [bp-01eh], dx                    ; 89 56 e2
    mov dx, word [bp-00220h]                  ; 8b 96 e0 fd
    mov word [bp-01ah], dx                    ; 89 56 e6
    mov dx, word [bp-0021ah]                  ; 8b 96 e6 fd
    mov word [bp-020h], dx                    ; 89 56 e0
    mov dx, word [bp-001aeh]                  ; 8b 96 52 fe
    mov word [bp-016h], dx                    ; 89 56 ea
    mov di, word [bp-001ach]                  ; 8b be 54 fe
    cmp di, 00fffh                            ; 81 ff ff 0f
    jne short 083cfh                          ; 75 10
    cmp dx, strict byte 0ffffh                ; 83 fa ff
    jne short 083cfh                          ; 75 0b
    mov dx, word [bp-0015eh]                  ; 8b 96 a2 fe
    mov word [bp-016h], dx                    ; 89 56 ea
    mov di, word [bp-0015ch]                  ; 8b be a4 fe
    movzx bx, byte [bp-00ah]                  ; 0f b6 5e f6
    mov es, [bp-012h]                         ; 8e 46 ee
    add bx, word [bp-014h]                    ; 03 5e ec
    mov ah, byte [bp-008h]                    ; 8a 66 f8
    mov byte [es:bx+001e9h], ah               ; 26 88 a7 e9 01
    movzx dx, byte [bp-00ch]                  ; 0f b6 56 f4
    imul dx, dx, strict byte 00018h           ; 6b d2 18
    mov si, word [bp-014h]                    ; 8b 76 ec
    add si, dx                                ; 01 d6
    mov word [es:si+01eh], 0ff05h             ; 26 c7 44 1e 05 ff
    mov byte [es:si+020h], al                 ; 26 88 44 20
    mov byte [es:si+021h], 000h               ; 26 c6 44 21 00
    mov word [es:si+024h], 00200h             ; 26 c7 44 24 00 02
    mov byte [es:si+023h], 001h               ; 26 c6 44 23 01
    mov ax, word [bp-016h]                    ; 8b 46 ea
    mov word [es:si+032h], ax                 ; 26 89 44 32
    mov word [es:si+034h], di                 ; 26 89 7c 34
    mov ax, word [bp-01ah]                    ; 8b 46 e6
    mov word [es:si+02ch], ax                 ; 26 89 44 2c
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    mov word [es:si+02eh], ax                 ; 26 89 44 2e
    mov ax, word [bp-020h]                    ; 8b 46 e0
    mov word [es:si+030h], ax                 ; 26 89 44 30
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    cmp AL, strict byte 001h                  ; 3c 01
    jc short 0843ah                           ; 72 0c
    jbe short 08442h                          ; 76 12
    cmp AL, strict byte 003h                  ; 3c 03
    je short 0844ah                           ; 74 16
    cmp AL, strict byte 002h                  ; 3c 02
    je short 08446h                           ; 74 0e
    jmp short 08491h                          ; eb 57
    test al, al                               ; 84 c0
    jne short 08491h                          ; 75 53
    mov DL, strict byte 040h                  ; b2 40
    jmp short 0844ch                          ; eb 0a
    mov DL, strict byte 048h                  ; b2 48
    jmp short 0844ch                          ; eb 06
    mov DL, strict byte 050h                  ; b2 50
    jmp short 0844ch                          ; eb 02
    mov DL, strict byte 058h                  ; b2 58
    mov al, dl                                ; 88 d0
    add AL, strict byte 007h                  ; 04 07
    movzx bx, al                              ; 0f b6 d8
    mov ax, bx                                ; 89 d8
    call 0165ch                               ; e8 04 92
    test al, al                               ; 84 c0
    je short 08491h                           ; 74 35
    mov al, dl                                ; 88 d0
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    xor ah, ah                                ; 30 e4
    call 0165ch                               ; e8 f7 91
    xor ah, ah                                ; 30 e4
    mov cx, ax                                ; 89 c1
    sal cx, 008h                              ; c1 e1 08
    movzx ax, dl                              ; 0f b6 c2
    call 0165ch                               ; e8 ea 91
    xor ah, ah                                ; 30 e4
    add ax, cx                                ; 01 c8
    mov word [bp-022h], ax                    ; 89 46 de
    mov al, dl                                ; 88 d0
    add AL, strict byte 002h                  ; 04 02
    xor ah, ah                                ; 30 e4
    call 0165ch                               ; e8 da 91
    xor ah, ah                                ; 30 e4
    mov word [bp-01ch], ax                    ; 89 46 e4
    mov ax, bx                                ; 89 d8
    call 0165ch                               ; e8 d0 91
    movzx dx, al                              ; 0f b6 d0
    jmp short 084a0h                          ; eb 0f
    mov ax, word [bp-01eh]                    ; 8b 46 e2
    mov word [bp-022h], ax                    ; 89 46 de
    mov ax, word [bp-01ah]                    ; 8b 46 e6
    mov word [bp-01ch], ax                    ; 89 46 e4
    mov dx, word [bp-020h]                    ; 8b 56 e0
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 21 94
    push di                                   ; 57
    push word [bp-016h]                       ; ff 76 ea
    push dx                                   ; 52
    push word [bp-01ch]                       ; ff 76 e4
    push word [bp-022h]                       ; ff 76 de
    push word [bp-020h]                       ; ff 76 e0
    push word [bp-01ah]                       ; ff 76 e6
    push word [bp-01eh]                       ; ff 76 e2
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    push ax                                   ; 50
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    push ax                                   ; 50
    push 00c10h                               ; 68 10 0c
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 3c 94
    add sp, strict byte 00018h                ; 83 c4 18
    movzx ax, byte [bp-00ch]                  ; 0f b6 46 f4
    imul ax, ax, strict byte 00018h           ; 6b c0 18
    les si, [bp-014h]                         ; c4 76 ec
    add si, ax                                ; 01 c6
    mov ax, word [bp-01ch]                    ; 8b 46 e4
    mov word [es:si+026h], ax                 ; 26 89 44 26
    mov ax, word [bp-022h]                    ; 8b 46 de
    mov word [es:si+028h], ax                 ; 26 89 44 28
    mov word [es:si+02ah], dx                 ; 26 89 54 2a
    mov bx, word [bp-014h]                    ; 8b 5e ec
    mov dl, byte [es:bx+0019eh]               ; 26 8a 97 9e 01
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    add AL, strict byte 00ch                  ; 04 0c
    movzx bx, dl                              ; 0f b6 da
    add bx, word [bp-014h]                    ; 03 5e ec
    mov byte [es:bx+0019fh], al               ; 26 88 87 9f 01
    db  0feh, 0c2h
    ; inc dl                                    ; fe c2
    mov bx, word [bp-014h]                    ; 8b 5e ec
    mov byte [es:bx+0019eh], dl               ; 26 88 97 9e 01
    mov dx, strict word 00075h                ; ba 75 00
    mov ax, strict word 00040h                ; b8 40 00
    call 01600h                               ; e8 e3 90
    mov dl, al                                ; 88 c2
    db  0feh, 0c2h
    ; inc dl                                    ; fe c2
    movzx bx, dl                              ; 0f b6 da
    mov dx, strict word 00075h                ; ba 75 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0160eh                               ; e8 e1 90
    jmp near 085c8h                           ; e9 98 00
    cmp dx, 0eb14h                            ; 81 fa 14 eb
    jne near 085c8h                           ; 0f 85 90 00
    cmp bx, 00101h                            ; 81 fb 01 01
    jne near 085c8h                           ; 0f 85 88 00
    mov es, [bp-00eh]                         ; 8e 46 f2
    db  066h, 026h, 0c7h, 004h, 000h, 000h, 000h, 000h
    ; mov dword [es:si], strict dword 000000000h ; 66 26 c7 04 00 00 00 00
    lea dx, [bp-00226h]                       ; 8d 96 da fd
    mov word [es:si+004h], dx                 ; 26 89 54 04
    mov [es:si+006h], ss                      ; 26 8c 54 06
    db  066h, 026h, 0c7h, 044h, 00ah, 001h, 000h, 000h, 002h
    ; mov dword [es:si+00ah], strict dword 002000001h ; 66 26 c7 44 0a 01 00 00 02
    mov bx, 000a1h                            ; bb a1 00
    mov ax, si                                ; 89 f0
    mov dx, word [bp-024h]                    ; 8b 56 dc
    call 07a8ah                               ; e8 1f f5
    test byte [bp-00226h], 080h               ; f6 86 da fd 80
    db  00fh, 095h, 0c0h
    ; setne al                                  ; 0f 95 c0
    xor ah, ah                                ; 30 e4
    mov dx, ax                                ; 89 c2
    movzx bx, byte [bp-00ah]                  ; 0f b6 5e f6
    mov es, [bp-024h]                         ; 8e 46 dc
    add bx, si                                ; 01 f3
    mov al, byte [bp-008h]                    ; 8a 46 f8
    mov byte [es:bx+001e9h], al               ; 26 88 87 e9 01
    movzx si, cl                              ; 0f b6 f1
    imul si, si, strict byte 00018h           ; 6b f6 18
    add si, 00122h                            ; 81 c6 22 01
    mov word [es:si+01eh], 00505h             ; 26 c7 44 1e 05 05
    mov byte [es:si+020h], dl                 ; 26 88 54 20
    mov word [es:si+024h], 00800h             ; 26 c7 44 24 00 08
    les bx, [bp-014h]                         ; c4 5e ec
    mov dl, byte [es:bx+001afh]               ; 26 8a 97 af 01
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    add AL, strict byte 00ch                  ; 04 0c
    movzx bx, dl                              ; 0f b6 da
    mov es, [bp-024h]                         ; 8e 46 dc
    add bx, 00122h                            ; 81 c3 22 01
    mov byte [es:bx+001b0h], al               ; 26 88 87 b0 01
    db  0feh, 0c2h
    ; inc dl                                    ; fe c2
    les bx, [bp-014h]                         ; c4 5e ec
    mov byte [es:bx+001afh], dl               ; 26 88 97 af 01
    inc byte [bp-00ah]                        ; fe 46 f6
    mov al, byte [bp-00ah]                    ; 8a 46 f6
    les bx, [bp-014h]                         ; c4 5e ec
    mov byte [es:bx+001edh], al               ; 26 88 87 ed 01
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_mem_alloc_:                             ; 0xf85de LB 0x43
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    mov dx, 00413h                            ; ba 13 04
    xor ax, ax                                ; 31 c0
    call 0161ch                               ; e8 2e 90
    test ax, ax                               ; 85 c0
    je short 08617h                           ; 74 25
    dec ax                                    ; 48
    mov bx, ax                                ; 89 c3
    xor dx, dx                                ; 31 d2
    mov cx, strict word 0000ah                ; b9 0a 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 085fah                               ; e2 fa
    mov si, ax                                ; 89 c6
    mov di, dx                                ; 89 d7
    mov cx, strict word 00004h                ; b9 04 00
    shr di, 1                                 ; d1 ef
    rcr si, 1                                 ; d1 de
    loop 08607h                               ; e2 fa
    mov dx, 00413h                            ; ba 13 04
    xor ax, ax                                ; 31 c0
    call 0162ah                               ; e8 15 90
    mov ax, si                                ; 89 f0
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
ahci_hba_init_:                              ; 0xf8621 LB 0x125
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov si, ax                                ; 89 c6
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, strict word 00040h                ; b8 40 00
    call 0161ch                               ; e8 e5 8f
    mov bx, 00122h                            ; bb 22 01
    mov word [bp-010h], ax                    ; 89 46 f0
    mov ax, strict word 00010h                ; b8 10 00
    xor cx, cx                                ; 31 c9
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea dx, [si+004h]                         ; 8d 54 04
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    call 085deh                               ; e8 82 ff
    mov di, ax                                ; 89 c7
    test ax, ax                               ; 85 c0
    je near 08725h                            ; 0f 84 c1 00
    mov es, [bp-010h]                         ; 8e 46 f0
    mov word [es:bx+001eeh], di               ; 26 89 bf ee 01
    mov byte [es:bx+001edh], 000h             ; 26 c6 87 ed 01 00
    xor bx, bx                                ; 31 db
    mov es, di                                ; 8e c7
    mov byte [es:bx+00262h], 0ffh             ; 26 c6 87 62 02 ff
    mov word [es:bx+00260h], si               ; 26 89 b7 60 02
    db  066h, 026h, 0c7h, 087h, 064h, 002h, 000h, 0c0h, 00ch, 000h
    ; mov dword [es:bx+00264h], strict dword 0000cc000h ; 66 26 c7 87 64 02 00 c0 0c 00
    mov bx, strict word 00001h                ; bb 01 00
    xor cx, cx                                ; 31 c9
    mov dx, strict word 00004h                ; ba 04 00
    mov ax, si                                ; 89 f0
    call 078b9h                               ; e8 21 f2
    mov ax, strict word 00004h                ; b8 04 00
    xor cx, cx                                ; 31 c9
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    lea bx, [si+004h]                         ; 8d 5c 04
    mov dx, bx                                ; 89 da
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    test AL, strict byte 001h                 ; a8 01
    jne short 08698h                          ; 75 de
    xor ax, ax                                ; 31 c0
    xor cx, cx                                ; 31 c9
    mov dx, si                                ; 89 f2
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    mov dx, bx                                ; 89 da
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    push strict byte 00000h                   ; 6a 00
    mov bx, strict word 0001fh                ; bb 1f 00
    xor cx, cx                                ; 31 c9
    call 0797ch                               ; e8 9e f2
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    mov byte [bp-00eh], al                    ; 88 46 f2
    mov byte [bp-00ch], 000h                  ; c6 46 f4 00
    jmp short 086f2h                          ; eb 09
    inc byte [bp-00ch]                        ; fe 46 f4
    cmp byte [bp-00ch], 020h                  ; 80 7e f4 20
    jnc short 08723h                          ; 73 31
    movzx cx, byte [bp-00ch]                  ; 0f b6 4e f4
    mov ax, strict word 00001h                ; b8 01 00
    xor dx, dx                                ; 31 d2
    jcxz 08703h                               ; e3 06
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 086fdh                               ; e2 fa
    mov bx, ax                                ; 89 c3
    mov cx, dx                                ; 89 d1
    mov dx, strict word 0000ch                ; ba 0c 00
    mov ax, si                                ; 89 f0
    call 07943h                               ; e8 34 f2
    test al, al                               ; 84 c0
    je short 086e9h                           ; 74 d6
    movzx bx, byte [bp-00ch]                  ; 0f b6 5e f4
    xor ax, ax                                ; 31 c0
    mov dx, di                                ; 89 fa
    call 08215h                               ; e8 f7 fa
    dec byte [bp-00eh]                        ; fe 4e f2
    jne short 086e9h                          ; 75 c6
    xor ax, ax                                ; 31 c0
    lea sp, [bp-00ah]                         ; 8d 66 f6
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
    or ax, word [di]                          ; 0b 05
    add AL, strict byte 003h                  ; 04 03
    add al, byte [bx+di]                      ; 02 01
    add byte [di], dl                         ; 00 15
    mov bl, dh                                ; 88 f3
    xchg cx, di                               ; 87 f9
    xchg di, di                               ; 87 ff
    xchg word [di], ax                        ; 87 05
    mov byte [bp+di], cl                      ; 88 0b
    mov byte [bx+di], dl                      ; 88 11
    mov byte [di], dl                         ; 88 15
    db  088h
_ahci_init:                                  ; 0xf8746 LB 0xfe
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 00006h                ; 83 ec 06
    mov ax, 00601h                            ; b8 01 06
    mov dx, strict word 00001h                ; ba 01 00
    call 08c64h                               ; e8 0d 05
    mov dx, ax                                ; 89 c2
    cmp ax, strict word 0ffffh                ; 3d ff ff
    je near 0883dh                            ; 0f 84 dd 00
    xor al, al                                ; 30 c0
    shr ax, 008h                              ; c1 e8 08
    mov byte [bp-00ah], al                    ; 88 46 f6
    mov byte [bp-008h], dl                    ; 88 56 f8
    xor dh, dh                                ; 30 f6
    xor ah, ah                                ; 30 e4
    mov bx, strict word 00034h                ; bb 34 00
    call 08c8fh                               ; e8 1a 05
    mov cl, al                                ; 88 c1
    test cl, cl                               ; 84 c9
    je short 0879eh                           ; 74 23
    movzx bx, cl                              ; 0f b6 d9
    movzx di, byte [bp-008h]                  ; 0f b6 7e f8
    movzx si, byte [bp-00ah]                  ; 0f b6 76 f6
    mov dx, di                                ; 89 fa
    mov ax, si                                ; 89 f0
    call 08c8fh                               ; e8 02 05
    cmp AL, strict byte 012h                  ; 3c 12
    je short 0879eh                           ; 74 0d
    mov al, cl                                ; 88 c8
    db  0feh, 0c0h
    ; inc al                                    ; fe c0
    movzx bx, al                              ; 0f b6 d8
    mov dx, di                                ; 89 fa
    mov ax, si                                ; 89 f0
    jmp short 08772h                          ; eb d4
    test cl, cl                               ; 84 c9
    je near 0883dh                            ; 0f 84 99 00
    add cl, 002h                              ; 80 c1 02
    movzx bx, cl                              ; 0f b6 d9
    movzx di, byte [bp-008h]                  ; 0f b6 7e f8
    movzx si, byte [bp-00ah]                  ; 0f b6 76 f6
    mov dx, di                                ; 89 fa
    mov ax, si                                ; 89 f0
    call 08c8fh                               ; e8 d6 04
    cmp AL, strict byte 010h                  ; 3c 10
    jne near 0883dh                           ; 0f 85 7e 00
    mov byte [bp-006h], 000h                  ; c6 46 fa 00
    mov al, cl                                ; 88 c8
    add AL, strict byte 002h                  ; 04 02
    movzx bx, al                              ; 0f b6 d8
    mov dx, di                                ; 89 fa
    mov ax, si                                ; 89 f0
    call 08cb3h                               ; e8 e2 04
    mov dx, ax                                ; 89 c2
    and ax, strict word 0000fh                ; 25 0f 00
    sub ax, strict word 00004h                ; 2d 04 00
    cmp ax, strict word 0000bh                ; 3d 0b 00
    jnbe short 08815h                         ; 77 37
    push CS                                   ; 0e
    pop ES                                    ; 07
    mov cx, strict word 00008h                ; b9 08 00
    mov di, 0872fh                            ; bf 2f 87
    repne scasb                               ; f2 ae
    sal cx, 1                                 ; d1 e1
    mov di, cx                                ; 89 cf
    mov ax, word [cs:di-078cah]               ; 2e 8b 85 36 87
    jmp ax                                    ; ff e0
    mov byte [bp-006h], 010h                  ; c6 46 fa 10
    jmp short 08815h                          ; eb 1c
    mov byte [bp-006h], 014h                  ; c6 46 fa 14
    jmp short 08815h                          ; eb 16
    mov byte [bp-006h], 018h                  ; c6 46 fa 18
    jmp short 08815h                          ; eb 10
    mov byte [bp-006h], 01ch                  ; c6 46 fa 1c
    jmp short 08815h                          ; eb 0a
    mov byte [bp-006h], 020h                  ; c6 46 fa 20
    jmp short 08815h                          ; eb 04
    mov byte [bp-006h], 024h                  ; c6 46 fa 24
    mov si, dx                                ; 89 d6
    shr si, 004h                              ; c1 ee 04
    sal si, 002h                              ; c1 e6 02
    mov al, byte [bp-006h]                    ; 8a 46 fa
    test al, al                               ; 84 c0
    je short 0883dh                           ; 74 19
    movzx bx, al                              ; 0f b6 d8
    movzx dx, byte [bp-008h]                  ; 0f b6 56 f8
    movzx ax, byte [bp-00ah]                  ; 0f b6 46 f6
    call 08cd5h                               ; e8 a3 04
    test AL, strict byte 001h                 ; a8 01
    je short 0883dh                           ; 74 07
    and AL, strict byte 0f0h                  ; 24 f0
    add ax, si                                ; 01 f0
    call 08621h                               ; e8 e4 fd
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
apm_out_str_:                                ; 0xf8844 LB 0x39
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    mov bx, ax                                ; 89 c3
    cmp byte [bx], 000h                       ; 80 3f 00
    je short 08859h                           ; 74 0a
    mov al, byte [bx]                         ; 8a 07
    out DX, AL                                ; ee
    inc bx                                    ; 43
    mov al, byte [bx]                         ; 8a 07
    db  00ah, 0c0h
    ; or al, al                                 ; 0a c0
    jne short 08851h                          ; 75 f8
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
    mov AL, byte [06c88h]                     ; a0 88 6c
    mov word [bp+si-03278h], si               ; 89 b2 88 cd
    mov byte [si-077h], ch                    ; 88 6c 89
    clc                                       ; f8
    mov byte [si-077h], ch                    ; 88 6c 89
    std                                       ; fd
    mov byte [bx+di-077h], al                 ; 88 41 89
    inc cx                                    ; 41
    mov word [bx+di-077h], ax                 ; 89 41 89
    cmp AL, strict byte 089h                  ; 3c 89
    inc cx                                    ; 41
    mov word [bx+di-077h], ax                 ; 89 41 89
    db  035h
    db  089h
_apm_function:                               ; 0xf887d LB 0xf5
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    and byte [bp+018h], 0feh                  ; 80 66 18 fe
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    cmp ax, strict word 0000eh                ; 3d 0e 00
    jnbe near 08941h                          ; 0f 87 b0 00
    mov bx, ax                                ; 89 c3
    add bx, ax                                ; 01 c3
    mov dx, word [bp+018h]                    ; 8b 56 18
    or dl, 001h                               ; 80 ca 01
    jmp word [cs:bx-077a1h]                   ; 2e ff a7 5f 88
    mov word [bp+012h], 00102h                ; c7 46 12 02 01
    mov word [bp+00ch], 0504dh                ; c7 46 0c 4d 50
    mov word [bp+010h], strict word 00003h    ; c7 46 10 03 00
    jmp near 0896ch                           ; e9 ba 00
    mov word [bp+012h], 0f000h                ; c7 46 12 00 f0
    mov word [bp+00ch], 08ec4h                ; c7 46 0c c4 8e
    mov word [bp+010h], 0f000h                ; c7 46 10 00 f0
    mov ax, strict word 0fff0h                ; b8 f0 ff
    mov word [bp+006h], ax                    ; 89 46 06
    mov word [bp+004h], ax                    ; 89 46 04
    jmp near 0896ch                           ; e9 9f 00
    mov word [bp+012h], 0f000h                ; c7 46 12 00 f0
    mov word [bp+00ch], 0da40h                ; c7 46 0c 40 da
    mov ax, 0f000h                            ; b8 00 f0
    mov word [bp+010h], ax                    ; 89 46 10
    mov word [bp+00eh], ax                    ; 89 46 0e
    mov ax, strict word 0fff0h                ; b8 f0 ff
    mov word [bp+006h], ax                    ; 89 46 06
    mov word [bp+004h], ax                    ; 89 46 04
    xor bx, bx                                ; 31 db
    sal ebx, 010h                             ; 66 c1 e3 10
    mov si, ax                                ; 89 c6
    sal esi, 010h                             ; 66 c1 e6 10
    jmp near 0896ch                           ; e9 74 00
    sti                                       ; fb
    hlt                                       ; f4
    jmp near 0896ch                           ; e9 6f 00
    cmp word [bp+010h], strict byte 00003h    ; 83 7e 10 03
    je short 08922h                           ; 74 1f
    cmp word [bp+010h], strict byte 00002h    ; 83 7e 10 02
    je short 0891ah                           ; 74 11
    cmp word [bp+010h], strict byte 00001h    ; 83 7e 10 01
    jne short 0892ah                          ; 75 1b
    mov dx, 08900h                            ; ba 00 89
    mov ax, 00c48h                            ; b8 48 0c
    call 08844h                               ; e8 2c ff
    jmp short 0896ch                          ; eb 52
    mov dx, 08900h                            ; ba 00 89
    mov ax, 00c50h                            ; b8 50 0c
    jmp short 08915h                          ; eb f3
    mov dx, 08900h                            ; ba 00 89
    mov ax, 00c58h                            ; b8 58 0c
    jmp short 08915h                          ; eb eb
    or ah, 00ah                               ; 80 cc 0a
    mov word [bp+012h], ax                    ; 89 46 12
    mov word [bp+018h], dx                    ; 89 56 18
    jmp short 0896ch                          ; eb 37
    mov word [bp+012h], 00102h                ; c7 46 12 02 01
    jmp short 0896ch                          ; eb 30
    or ah, 080h                               ; 80 cc 80
    jmp short 0892dh                          ; eb ec
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 80 8f
    push word [bp+00ch]                       ; ff 76 0c
    push word [bp+012h]                       ; ff 76 12
    push 00c61h                               ; 68 61 0c
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 b3 8f
    add sp, strict byte 00008h                ; 83 c4 08
    mov ax, word [bp+012h]                    ; 8b 46 12
    xor ah, ah                                ; 30 e4
    or ah, 00ch                               ; 80 cc 0c
    mov word [bp+012h], ax                    ; 89 46 12
    or byte [bp+018h], 001h                   ; 80 4e 18 01
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
pci16_select_reg_:                           ; 0xf8972 LB 0x24
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    and dl, 0fch                              ; 80 e2 fc
    mov bx, dx                                ; 89 d3
    mov dx, 00cf8h                            ; ba f8 0c
    movzx eax, ax                             ; 66 0f b7 c0
    sal eax, 008h                             ; 66 c1 e0 08
    or eax, strict dword 080000000h           ; 66 0d 00 00 00 80
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    out DX, eax                               ; 66 ef
    lea sp, [bp-002h]                         ; 8d 66 fe
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
pci16_find_device_:                          ; 0xf8996 LB 0xf7
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    sub sp, strict byte 0000ch                ; 83 ec 0c
    push ax                                   ; 50
    push dx                                   ; 52
    mov si, bx                                ; 89 de
    mov di, cx                                ; 89 cf
    test cx, cx                               ; 85 c9
    xor bx, bx                                ; 31 db
    mov byte [bp-008h], 000h                  ; c6 46 f8 00
    test bl, 007h                             ; f6 c3 07
    jne short 089deh                          ; 75 2d
    mov dx, strict word 0000eh                ; ba 0e 00
    mov ax, bx                                ; 89 d8
    call 08972h                               ; e8 b9 ff
    mov dx, 00cfeh                            ; ba fe 0c
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    mov byte [bp-006h], al                    ; 88 46 fa
    cmp AL, strict byte 0ffh                  ; 3c ff
    jne short 089cch                          ; 75 06
    add bx, strict byte 00008h                ; 83 c3 08
    jmp near 08a5fh                           ; e9 93 00
    test byte [bp-006h], 080h                 ; f6 46 fa 80
    je short 089d9h                           ; 74 07
    mov word [bp-00ah], strict word 00001h    ; c7 46 f6 01 00
    jmp short 089deh                          ; eb 05
    mov word [bp-00ah], strict word 00008h    ; c7 46 f6 08 00
    mov al, byte [bp-006h]                    ; 8a 46 fa
    and AL, strict byte 007h                  ; 24 07
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 08a06h                          ; 75 1f
    mov ax, bx                                ; 89 d8
    shr ax, 008h                              ; c1 e8 08
    test ax, ax                               ; 85 c0
    jne short 08a06h                          ; 75 16
    mov dx, strict word 0001ah                ; ba 1a 00
    mov ax, bx                                ; 89 d8
    call 08972h                               ; e8 7a ff
    mov dx, 00cfeh                            ; ba fe 0c
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    cmp al, byte [bp-008h]                    ; 3a 46 f8
    jbe short 08a06h                          ; 76 03
    mov byte [bp-008h], al                    ; 88 46 f8
    test di, di                               ; 85 ff
    je short 08a0fh                           ; 74 05
    mov dx, strict word 00008h                ; ba 08 00
    jmp short 08a11h                          ; eb 02
    xor dx, dx                                ; 31 d2
    mov ax, bx                                ; 89 d8
    call 08972h                               ; e8 5c ff
    mov dx, 00cfch                            ; ba fc 0c
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov word [bp-00ch], dx                    ; 89 56 f4
    mov word [bp-010h], strict word 00000h    ; c7 46 f0 00 00
    test di, di                               ; 85 ff
    je short 08a40h                           ; 74 0f
    mov cx, strict word 00008h                ; b9 08 00
    shr dx, 1                                 ; d1 ea
    rcr ax, 1                                 ; d1 d8
    loop 08a34h                               ; e2 fa
    mov word [bp-00eh], ax                    ; 89 46 f2
    mov word [bp-00ch], dx                    ; 89 56 f4
    mov ax, word [bp-00ch]                    ; 8b 46 f4
    cmp ax, word [bp-014h]                    ; 3b 46 ec
    jne short 08a50h                          ; 75 08
    mov ax, word [bp-00eh]                    ; 8b 46 f2
    cmp ax, word [bp-012h]                    ; 3b 46 ee
    je short 08a56h                           ; 74 06
    cmp word [bp-010h], strict byte 00000h    ; 83 7e f0 00
    je short 08a5ch                           ; 74 06
    dec si                                    ; 4e
    cmp si, strict byte 0ffffh                ; 83 fe ff
    je short 08a6eh                           ; 74 12
    add bx, word [bp-00ah]                    ; 03 5e f6
    mov dx, bx                                ; 89 da
    shr dx, 008h                              ; c1 ea 08
    movzx ax, byte [bp-008h]                  ; 0f b6 46 f8
    cmp dx, ax                                ; 39 c2
    jbe near 089ach                           ; 0f 86 3e ff
    cmp si, strict byte 0ffffh                ; 83 fe ff
    jne short 08a77h                          ; 75 04
    mov ax, bx                                ; 89 d8
    jmp short 08a7ah                          ; eb 03
    mov ax, strict word 0ffffh                ; b8 ff ff
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
    outsw                                     ; 6f
    mov cx, word [bx+di-06375h]               ; 8b 89 8b 9c
    mov si, word [bx+di-03b75h]               ; 8b b1 8b c4
    db  08bh, 0d7h
    ; mov dx, di                                ; 8b d7
    db  08bh
_pci16_function:                             ; 0xf8a8d LB 0x1d7
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    push ax                                   ; 50
    push ax                                   ; 50
    and word [bp+020h], 000ffh                ; 81 66 20 ff 00
    and word [bp+02ch], strict byte 0fffeh    ; 83 66 2c fe
    mov bx, word [bp+020h]                    ; 8b 5e 20
    xor bh, bh                                ; 30 ff
    mov ax, word [bp+020h]                    ; 8b 46 20
    xor ah, ah                                ; 30 e4
    cmp bx, strict byte 00003h                ; 83 fb 03
    jc short 08ac6h                           ; 72 1a
    jbe short 08b1eh                          ; 76 70
    cmp bx, strict byte 0000eh                ; 83 fb 0e
    je near 08bebh                            ; 0f 84 36 01
    cmp bx, strict byte 00008h                ; 83 fb 08
    jc near 08c30h                            ; 0f 82 74 01
    cmp bx, strict byte 0000dh                ; 83 fb 0d
    jbe near 08b43h                           ; 0f 86 80 00
    jmp near 08c30h                           ; e9 6a 01
    cmp bx, strict byte 00002h                ; 83 fb 02
    je short 08aeeh                           ; 74 23
    cmp bx, strict byte 00001h                ; 83 fb 01
    jne near 08c30h                           ; 0f 85 5e 01
    mov word [bp+020h], strict word 00001h    ; c7 46 20 01 00
    mov word [bp+014h], 00210h                ; c7 46 14 10 02
    mov word [bp+01ch], strict word 00000h    ; c7 46 1c 00 00
    mov word [bp+018h], 04350h                ; c7 46 18 50 43
    mov word [bp+01ah], 02049h                ; c7 46 1a 49 20
    jmp near 08c5dh                           ; e9 6f 01
    cmp word [bp+018h], strict byte 0ffffh    ; 83 7e 18 ff
    jne short 08afah                          ; 75 06
    or ah, 083h                               ; 80 cc 83
    jmp near 08c56h                           ; e9 5c 01
    mov bx, word [bp+008h]                    ; 8b 5e 08
    mov dx, word [bp+01ch]                    ; 8b 56 1c
    mov ax, word [bp+018h]                    ; 8b 46 18
    xor cx, cx                                ; 31 c9
    call 08996h                               ; e8 8e fe
    cmp ax, strict word 0ffffh                ; 3d ff ff
    jne short 08b18h                          ; 75 0b
    mov ax, word [bp+020h]                    ; 8b 46 20
    xor ah, ah                                ; 30 e4
    or ah, 086h                               ; 80 cc 86
    jmp near 08c56h                           ; e9 3e 01
    mov word [bp+014h], ax                    ; 89 46 14
    jmp near 08c5dh                           ; e9 3f 01
    mov bx, word [bp+008h]                    ; 8b 5e 08
    mov ax, word [bp+01ch]                    ; 8b 46 1c
    mov dx, word [bp+01eh]                    ; 8b 56 1e
    mov cx, strict word 00001h                ; b9 01 00
    call 08996h                               ; e8 69 fe
    cmp ax, strict word 0ffffh                ; 3d ff ff
    jne short 08b3dh                          ; 75 0b
    mov ax, word [bp+020h]                    ; 8b 46 20
    xor ah, ah                                ; 30 e4
    or ah, 086h                               ; 80 cc 86
    jmp near 08c56h                           ; e9 19 01
    mov word [bp+014h], ax                    ; 89 46 14
    jmp near 08c5dh                           ; e9 1a 01
    cmp word [bp+004h], 00100h                ; 81 7e 04 00 01
    jc short 08b50h                           ; 72 06
    or ah, 087h                               ; 80 cc 87
    jmp near 08c56h                           ; e9 06 01
    mov dx, word [bp+004h]                    ; 8b 56 04
    mov ax, word [bp+014h]                    ; 8b 46 14
    call 08972h                               ; e8 19 fe
    mov bx, word [bp+020h]                    ; 8b 5e 20
    xor bh, bh                                ; 30 ff
    sub bx, strict byte 00008h                ; 83 eb 08
    cmp bx, strict byte 00005h                ; 83 fb 05
    jnbe near 08c5dh                          ; 0f 87 f5 00
    add bx, bx                                ; 01 db
    jmp word [cs:bx-0757fh]                   ; 2e ff a7 81 8a
    mov bx, word [bp+01ch]                    ; 8b 5e 1c
    xor bl, bl                                ; 30 db
    mov dx, word [bp+004h]                    ; 8b 56 04
    and dx, strict byte 00003h                ; 83 e2 03
    add dx, 00cfch                            ; 81 c2 fc 0c
    in AL, DX                                 ; ec
    db  02ah, 0e4h
    ; sub ah, ah                                ; 2a e4
    or bx, ax                                 ; 09 c3
    mov word [bp+01ch], bx                    ; 89 5e 1c
    jmp near 08c5dh                           ; e9 d4 00
    mov dx, word [bp+004h]                    ; 8b 56 04
    xor dh, dh                                ; 30 f6
    and dl, 002h                              ; 80 e2 02
    add dx, 00cfch                            ; 81 c2 fc 0c
    in ax, DX                                 ; ed
    mov word [bp+01ch], ax                    ; 89 46 1c
    jmp near 08c5dh                           ; e9 c1 00
    mov dx, 00cfch                            ; ba fc 0c
    in eax, DX                                ; 66 ed
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    shr eax, 010h                             ; 66 c1 e8 10
    xchg dx, ax                               ; 92
    mov word [bp+01ch], ax                    ; 89 46 1c
    mov word [bp+01eh], dx                    ; 89 56 1e
    jmp near 08c5dh                           ; e9 ac 00
    mov ax, word [bp+01ch]                    ; 8b 46 1c
    mov dx, word [bp+004h]                    ; 8b 56 04
    xor dh, dh                                ; 30 f6
    and dl, 003h                              ; 80 e2 03
    add dx, 00cfch                            ; 81 c2 fc 0c
    out DX, AL                                ; ee
    jmp near 08c5dh                           ; e9 99 00
    mov ax, word [bp+01ch]                    ; 8b 46 1c
    mov dx, word [bp+004h]                    ; 8b 56 04
    xor dh, dh                                ; 30 f6
    and dl, 002h                              ; 80 e2 02
    add dx, 00cfch                            ; 81 c2 fc 0c
    out DX, ax                                ; ef
    jmp near 08c5dh                           ; e9 86 00
    mov ax, word [bp+01ch]                    ; 8b 46 1c
    mov cx, word [bp+01eh]                    ; 8b 4e 1e
    mov dx, 00cfch                            ; ba fc 0c
    xchg cx, ax                               ; 91
    sal eax, 010h                             ; 66 c1 e0 10
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    out DX, eax                               ; 66 ef
    jmp short 08c5dh                          ; eb 72
    mov bx, word [bp+004h]                    ; 8b 5e 04
    mov es, [bp+026h]                         ; 8e 46 26
    mov word [bp-008h], bx                    ; 89 5e f8
    mov [bp-006h], es                         ; 8c 46 fa
    mov cx, word [word 00000h]                ; 8b 0e 00 00
    cmp cx, word [es:bx]                      ; 26 3b 0f
    jbe short 08c11h                          ; 76 11
    mov ax, word [bp+020h]                    ; 8b 46 20
    xor ah, ah                                ; 30 e4
    or ah, 089h                               ; 80 cc 89
    mov word [bp+020h], ax                    ; 89 46 20
    or word [bp+02ch], strict byte 00001h     ; 83 4e 2c 01
    jmp short 08c25h                          ; eb 14
    les di, [es:bx+002h]                      ; 26 c4 7f 02
    mov si, 0f2c0h                            ; be c0 f2
    mov dx, ds                                ; 8c da
    push DS                                   ; 1e
    mov ds, dx                                ; 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    mov word [bp+014h], 00a00h                ; c7 46 14 00 0a
    mov ax, word [00000h]                     ; a1 00 00
    les bx, [bp-008h]                         ; c4 5e f8
    mov word [es:bx], ax                      ; 26 89 07
    jmp short 08c5dh                          ; eb 2d
    mov bx, 00cd8h                            ; bb d8 0c
    mov cx, ds                                ; 8c d9
    mov ax, strict word 00004h                ; b8 04 00
    call 018cch                               ; e8 91 8c
    mov ax, word [bp+014h]                    ; 8b 46 14
    push ax                                   ; 50
    mov ax, word [bp+020h]                    ; 8b 46 20
    push ax                                   ; 50
    push 00c94h                               ; 68 94 0c
    push strict byte 00004h                   ; 6a 04
    call 0190dh                               ; e8 c2 8c
    add sp, strict byte 00008h                ; 83 c4 08
    mov ax, word [bp+020h]                    ; 8b 46 20
    xor ah, ah                                ; 30 e4
    or ah, 081h                               ; 80 cc 81
    mov word [bp+020h], ax                    ; 89 46 20
    or word [bp+02ch], strict byte 00001h     ; 83 4e 2c 01
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn                                      ; c3
pci_find_classcode_:                         ; 0xf8c64 LB 0x2b
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push cx                                   ; 51
    push si                                   ; 56
    mov cx, dx                                ; 89 d1
    xor si, si                                ; 31 f6
    mov dx, ax                                ; 89 c2
    mov ax, 0b103h                            ; b8 03 b1
    sal ecx, 010h                             ; 66 c1 e1 10
    db  08bh, 0cah
    ; mov cx, dx                                ; 8b ca
    int 01ah                                  ; cd 1a
    cmp ah, 000h                              ; 80 fc 00
    je near 08c85h                            ; 0f 84 03 00
    mov bx, strict word 0ffffh                ; bb ff ff
    mov ax, bx                                ; 89 d8
    lea sp, [bp-006h]                         ; 8d 66 fa
    pop si                                    ; 5e
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
pci_read_config_byte_:                       ; 0xf8c8f LB 0x24
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push di                                   ; 57
    movzx di, bl                              ; 0f b6 fb
    movzx bx, al                              ; 0f b6 d8
    sal bx, 008h                              ; c1 e3 08
    movzx ax, dl                              ; 0f b6 c2
    or bx, ax                                 ; 09 c3
    mov ax, 0b108h                            ; b8 08 b1
    int 01ah                                  ; cd 1a
    movzx ax, cl                              ; 0f b6 c1
    xor dx, dx                                ; 31 d2
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
pci_read_config_word_:                       ; 0xf8cb3 LB 0x22
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push di                                   ; 57
    movzx di, bl                              ; 0f b6 fb
    xor ah, ah                                ; 30 e4
    sal ax, 008h                              ; c1 e0 08
    movzx bx, dl                              ; 0f b6 da
    or bx, ax                                 ; 09 c3
    mov ax, 0b109h                            ; b8 09 b1
    int 01ah                                  ; cd 1a
    mov ax, cx                                ; 89 c8
    xor dx, dx                                ; 31 d2
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
pci_read_config_dword_:                      ; 0xf8cd5 LB 0x27
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push cx                                   ; 51
    push di                                   ; 57
    movzx di, bl                              ; 0f b6 fb
    movzx bx, al                              ; 0f b6 d8
    sal bx, 008h                              ; c1 e3 08
    movzx ax, dl                              ; 0f b6 c2
    or bx, ax                                 ; 09 c3
    mov ax, 0b10ah                            ; b8 0a b1
    int 01ah                                  ; cd 1a
    db  08bh, 0c1h
    ; mov ax, cx                                ; 8b c1
    shr ecx, 010h                             ; 66 c1 e9 10
    mov dx, cx                                ; 89 ca
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop cx                                    ; 59
    pop bp                                    ; 5d
    retn                                      ; c3
vds_is_present_:                             ; 0xf8cfc LB 0x1d
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, strict word 0007bh                ; bb 7b 00
    mov ax, strict word 00040h                ; b8 40 00
    mov es, ax                                ; 8e c0
    test byte [es:bx], 020h                   ; 26 f6 07 20
    je short 08d14h                           ; 74 06
    mov ax, strict word 00001h                ; b8 01 00
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
    xor ax, ax                                ; 31 c0
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
vds_real_to_lin_:                            ; 0xf8d19 LB 0x1e
    push bx                                   ; 53
    push cx                                   ; 51
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    mov bx, ax                                ; 89 c3
    mov ax, dx                                ; 89 d0
    xor dx, dx                                ; 31 d2
    mov cx, strict word 00004h                ; b9 04 00
    sal ax, 1                                 ; d1 e0
    rcl dx, 1                                 ; d1 d2
    loop 08d27h                               ; e2 fa
    xor cx, cx                                ; 31 c9
    add ax, bx                                ; 01 d8
    adc dx, cx                                ; 11 ca
    pop bp                                    ; 5d
    pop cx                                    ; 59
    pop bx                                    ; 5b
    retn                                      ; c3
vds_build_sg_list_:                          ; 0xf8d37 LB 0x79
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push si                                   ; 56
    push di                                   ; 57
    mov di, ax                                ; 89 c7
    mov si, dx                                ; 89 d6
    mov ax, bx                                ; 89 d8
    mov dx, cx                                ; 89 ca
    mov bx, word [bp+004h]                    ; 8b 5e 04
    mov es, si                                ; 8e c6
    mov word [es:di], bx                      ; 26 89 1d
    mov bx, word [bp+006h]                    ; 8b 5e 06
    mov word [es:di+002h], bx                 ; 26 89 5d 02
    call 08d19h                               ; e8 c3 ff
    mov es, si                                ; 8e c6
    mov word [es:di+004h], ax                 ; 26 89 45 04
    mov word [es:di+006h], dx                 ; 26 89 55 06
    mov word [es:di+008h], strict word 00000h ; 26 c7 45 08 00 00
    call 08cfch                               ; e8 93 ff
    test ax, ax                               ; 85 c0
    je short 08d80h                           ; 74 13
    mov es, si                                ; 8e c6
    mov ax, 08105h                            ; b8 05 81
    mov dx, strict word 00000h                ; ba 00 00
    int 04bh                                  ; cd 4b
    jc near 08d7dh                            ; 0f 82 02 00
    db  032h, 0c0h
    ; xor al, al                                ; 32 c0
    cbw                                       ; 98
    jmp short 08da7h                          ; eb 27
    mov es, si                                ; 8e c6
    mov word [es:di+00eh], strict word 00001h ; 26 c7 45 0e 01 00
    mov dx, word [es:di+004h]                 ; 26 8b 55 04
    mov ax, word [es:di+006h]                 ; 26 8b 45 06
    mov word [es:di+010h], dx                 ; 26 89 55 10
    mov word [es:di+012h], ax                 ; 26 89 45 12
    mov ax, word [bp+004h]                    ; 8b 46 04
    mov word [es:di+014h], ax                 ; 26 89 45 14
    mov ax, bx                                ; 89 d8
    mov word [es:di+016h], bx                 ; 26 89 5d 16
    xor ax, bx                                ; 31 d8
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bp                                    ; 5d
    retn 00004h                               ; c2 04 00
vds_free_sg_list_:                           ; 0xf8db0 LB 0x30
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    push bx                                   ; 53
    push di                                   ; 57
    mov bx, ax                                ; 89 c3
    call 08cfch                               ; e8 42 ff
    test ax, ax                               ; 85 c0
    je short 08dd1h                           ; 74 13
    mov di, bx                                ; 89 df
    mov es, dx                                ; 8e c2
    mov ax, 08106h                            ; b8 06 81
    mov dx, strict word 00000h                ; ba 00 00
    int 04bh                                  ; cd 4b
    jc near 08dd0h                            ; 0f 82 02 00
    db  032h, 0c0h
    ; xor al, al                                ; 32 c0
    cbw                                       ; 98
    mov es, dx                                ; 8e c2
    mov word [es:bx+00eh], strict word 00000h ; 26 c7 47 0e 00 00
    lea sp, [bp-004h]                         ; 8d 66 fc
    pop di                                    ; 5f
    pop bx                                    ; 5b
    pop bp                                    ; 5d
    retn                                      ; c3
__U4D:                                       ; 0xf8de0 LB 0x39
    pushfw                                    ; 9c
    push eax                                  ; 66 50
    push edx                                  ; 66 52
    push ecx                                  ; 66 51
    rol eax, 010h                             ; 66 c1 c0 10
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    ror eax, 010h                             ; 66 c1 c8 10
    db  066h, 033h, 0d2h
    ; xor edx, edx                              ; 66 33 d2
    shr ecx, 010h                             ; 66 c1 e9 10
    db  08bh, 0cbh
    ; mov cx, bx                                ; 8b cb
    div ecx                                   ; 66 f7 f1
    db  08bh, 0dah
    ; mov bx, dx                                ; 8b da
    pop ecx                                   ; 66 59
    shr edx, 010h                             ; 66 c1 ea 10
    db  08bh, 0cah
    ; mov cx, dx                                ; 8b ca
    pop edx                                   ; 66 5a
    ror eax, 010h                             ; 66 c1 c8 10
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    add sp, strict byte 00002h                ; 83 c4 02
    pop ax                                    ; 58
    rol eax, 010h                             ; 66 c1 c0 10
    popfw                                     ; 9d
    retn                                      ; c3
__U4M:                                       ; 0xf8e19 LB 0x31
    pushfw                                    ; 9c
    push eax                                  ; 66 50
    push edx                                  ; 66 52
    push ecx                                  ; 66 51
    rol eax, 010h                             ; 66 c1 c0 10
    db  08bh, 0c2h
    ; mov ax, dx                                ; 8b c2
    ror eax, 010h                             ; 66 c1 c8 10
    db  066h, 033h, 0d2h
    ; xor edx, edx                              ; 66 33 d2
    shr ecx, 010h                             ; 66 c1 e9 10
    db  08bh, 0cbh
    ; mov cx, bx                                ; 8b cb
    mul ecx                                   ; 66 f7 e1
    pop ecx                                   ; 66 59
    pop edx                                   ; 66 5a
    ror eax, 010h                             ; 66 c1 c8 10
    db  08bh, 0d0h
    ; mov dx, ax                                ; 8b d0
    add sp, strict byte 00002h                ; 83 c4 02
    pop ax                                    ; 58
    rol eax, 010h                             ; 66 c1 c0 10
    popfw                                     ; 9d
    retn                                      ; c3
_fmemset_:                                   ; 0xf8e4a LB 0xd
    push di                                   ; 57
    mov es, dx                                ; 8e c2
    db  08bh, 0f8h
    ; mov di, ax                                ; 8b f8
    xchg al, bl                               ; 86 d8
    rep stosb                                 ; f3 aa
    xchg al, bl                               ; 86 d8
    pop di                                    ; 5f
    retn                                      ; c3
_fmemcpy_:                                   ; 0xf8e57 LB 0x33
    push bp                                   ; 55
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    push di                                   ; 57
    push DS                                   ; 1e
    push si                                   ; 56
    mov es, dx                                ; 8e c2
    db  08bh, 0f8h
    ; mov di, ax                                ; 8b f8
    mov ds, cx                                ; 8e d9
    db  08bh, 0f3h
    ; mov si, bx                                ; 8b f3
    mov cx, word [bp+004h]                    ; 8b 4e 04
    rep movsb                                 ; f3 a4
    pop si                                    ; 5e
    pop DS                                    ; 1f
    pop di                                    ; 5f
    leave                                     ; c9
    retn                                      ; c3
    add byte [bx+si-05d72h], ah               ; 00 a0 8e a2
    mov fs, [bp-05972h]                       ; 8e a6 8e a6
    mov fs, [bp-05772h]                       ; 8e a6 8e a8
    mov gs, [bx+si-05572h]                    ; 8e a8 8e aa
    mov gs, [bp-05172h]                       ; 8e ae 8e ae
    db  08eh
    mov AL, strict byte 08eh                  ; b0 8e
    mov CH, strict byte 08eh                  ; b5 8e
    mov BH, strict byte 08eh                  ; b7 8e
apm_worker:                                  ; 0xf8e8a LB 0x3a
    sti                                       ; fb
    push ax                                   ; 50
    db  032h, 0e4h
    ; xor ah, ah                                ; 32 e4
    sub AL, strict byte 004h                  ; 2c 04
    db  08bh, 0e8h
    ; mov bp, ax                                ; 8b e8
    sal bp, 1                                 ; d1 e5
    cmp AL, strict byte 00dh                  ; 3c 0d
    pop ax                                    ; 58
    mov AH, strict byte 053h                  ; b4 53
    jnc short 08ec0h                          ; 73 25
    jmp word [cs:bp-07190h]                   ; 2e ff a6 70 8e
    jmp short 08ebeh                          ; eb 1c
    sti                                       ; fb
    hlt                                       ; f4
    jmp short 08ebeh                          ; eb 18
    jmp short 08ebeh                          ; eb 16
    jmp short 08ec0h                          ; eb 16
    mov AH, strict byte 080h                  ; b4 80
    jmp short 08ec2h                          ; eb 14
    jmp short 08ec0h                          ; eb 10
    mov ax, 00102h                            ; b8 02 01
    jmp short 08ebeh                          ; eb 09
    jmp short 08ebeh                          ; eb 07
    mov BL, strict byte 000h                  ; b3 00
    mov cx, strict word 00000h                ; b9 00 00
    jmp short 08ebeh                          ; eb 00
    clc                                       ; f8
    retn                                      ; c3
    mov AH, strict byte 009h                  ; b4 09
    stc                                       ; f9
    retn                                      ; c3
apm_pm16_entry:                              ; 0xf8ec4 LB 0x11
    mov AH, strict byte 002h                  ; b4 02
    push DS                                   ; 1e
    push bp                                   ; 55
    push CS                                   ; 0e
    pop bp                                    ; 5d
    add bp, strict byte 00008h                ; 83 c5 08
    mov ds, bp                                ; 8e dd
    call 08e8ah                               ; e8 b8 ff
    pop bp                                    ; 5d
    pop DS                                    ; 1f
    retf                                      ; cb

  ; Padding 0x4b2b bytes at 0xf8ed5
  times 19243 db 0

section BIOS32 progbits vstart=0xda00 align=1 ; size=0x3c9 class=CODE group=AUTO
bios32_service:                              ; 0xfda00 LB 0x26
    pushfw                                    ; 9c
    cmp bl, 000h                              ; 80 fb 00
    jne short 0da22h                          ; 75 1c
    cmp ax, 05024h                            ; 3d 24 50
    inc bx                                    ; 43
    dec cx                                    ; 49
    mov AL, strict byte 080h                  ; b0 80
    jne short 0da20h                          ; 75 11
    mov bx, strict word 00000h                ; bb 00 00
    db  00fh
    add byte [bx+di-01000h], bh               ; 00 b9 00 f0
    add byte [bx+si], al                      ; 00 00
    mov dx, 0da26h                            ; ba 26 da
    add byte [bx+si], al                      ; 00 00
    db  032h, 0c0h
    ; xor al, al                                ; 32 c0
    popfw                                     ; 9d
    retf                                      ; cb
    mov AL, strict byte 081h                  ; b0 81
    jmp short 0da20h                          ; eb fa
pcibios32_entry:                             ; 0xfda26 LB 0x1a
    pushfw                                    ; 9c
    cld                                       ; fc
    push ES                                   ; 06
    pushaw                                    ; 60
    call 0db73h                               ; e8 46 01
    add byte [bx+si], al                      ; 00 00
    popaw                                     ; 61
    pop ES                                    ; 07
    popfw                                     ; 9d
    retf                                      ; cb
    times 0xd db 0
apm_pm32_entry:                              ; 0xfda40 LB 0x21
    push bp                                   ; 55
    mov ebp, cs                               ; 66 8c cd
    push ebp                                  ; 66 55
    mov bp, 0da5fh                            ; bd 5f da
    add byte [bx+si], al                      ; 00 00
    push ebp                                  ; 66 55
    push CS                                   ; 0e
    pop bp                                    ; 5d
    add bp, strict byte 00008h                ; 83 c5 08
    push ebp                                  ; 66 55
    mov bp, 08ec6h                            ; bd c6 8e
    add byte [bx+si], al                      ; 00 00
    push ebp                                  ; 66 55
    mov AH, strict byte 003h                  ; b4 03
    db  066h, 0cbh
    ; retf                                      ; 66 cb
    pop bp                                    ; 5d
    retf                                      ; cb
pci32_select_reg_:                           ; 0xfda61 LB 0x1f
    push bx                                   ; 53
    push bp                                   ; 55
    mov bp, sp                                ; 89 e5
    and dl, 0fch                              ; 80 e2 fc
    mov bx, dx                                ; 89 d3
    mov dx, 00cf8h                            ; ba f8 0c
    add byte [bx+si], al                      ; 00 00
    db  00fh, 0b7h, 0c0h
    ; movzx ax, ax                              ; 0f b7 c0
    sal ax, 008h                              ; c1 e0 08
    or ax, strict word 00000h                 ; 0d 00 00
    add byte [bx+si-03c76h], al               ; 00 80 8a c3
    out DX, ax                                ; ef
    pop bp                                    ; 5d
    pop bx                                    ; 5b
    retn                                      ; c3
pci32_find_device_:                          ; 0xfda80 LB 0xf5
    push cx                                   ; 51
    push si                                   ; 56
    push di                                   ; 57
    enter 00014h, 000h                        ; c8 14 00 00
    push ax                                   ; 50
    mov cx, dx                                ; 89 d1
    mov si, bx                                ; 89 de
    test bx, bx                               ; 85 db
    xor bx, bx                                ; 31 db
    mov byte [di-004h], 000h                  ; c6 45 fc 00
    test bl, 007h                             ; f6 c3 07
    jne short 0dacfh                          ; 75 36
    db  00fh, 0b7h, 0c3h
    ; movzx ax, bx                              ; 0f b7 c3
    mov dx, strict word 0000eh                ; ba 0e 00
    add byte [bx+si], al                      ; 00 00
    call 0da5fh                               ; e8 bb ff
    db  0ffh
    db  0ffh
    mov dx, 00cfeh                            ; ba fe 0c
    add byte [bx+si], al                      ; 00 00
    db  02bh, 0c0h
    ; sub ax, ax                                ; 2b c0
    in AL, DX                                 ; ec
    mov byte [di-008h], al                    ; 88 45 f8
    cmp AL, strict byte 0ffh                  ; 3c ff
    jne short 0dabdh                          ; 75 08
    add bx, strict byte 00008h                ; 83 c3 08
    jmp near 0db45h                           ; e9 8a 00
    add byte [bx+si], al                      ; 00 00
    test byte [di-008h], 080h                 ; f6 45 f8 80
    je short 0dacah                           ; 74 07
    mov di, strict word 00001h                ; bf 01 00
    add byte [bx+si], al                      ; 00 00
    jmp short 0dacfh                          ; eb 05
    mov di, strict word 00008h                ; bf 08 00
    add byte [bx+si], al                      ; 00 00
    mov al, byte [di-008h]                    ; 8a 45 f8
    and AL, strict byte 007h                  ; 24 07
    cmp AL, strict byte 001h                  ; 3c 01
    jne short 0dafeh                          ; 75 26
    db  00fh, 0b7h, 0c3h
    ; movzx ax, bx                              ; 0f b7 c3
    mov dx, ax                                ; 89 c2
    sar dx, 008h                              ; c1 fa 08
    test dx, dx                               ; 85 d2
    jne short 0dafeh                          ; 75 1a
    mov dx, strict word 0001ah                ; ba 1a 00
    add byte [bx+si], al                      ; 00 00
    call 0da5fh                               ; e8 73 ff
    db  0ffh
    db  0ffh
    mov dx, 00cfeh                            ; ba fe 0c
    add byte [bx+si], al                      ; 00 00
    db  02bh, 0c0h
    ; sub ax, ax                                ; 2b c0
    in AL, DX                                 ; ec
    cmp al, byte [di-004h]                    ; 3a 45 fc
    jbe short 0dafeh                          ; 76 03
    mov byte [di-004h], al                    ; 88 45 fc
    test si, si                               ; 85 f6
    je short 0db09h                           ; 74 07
    mov ax, strict word 00008h                ; b8 08 00
    add byte [bx+si], al                      ; 00 00
    jmp short 0db0bh                          ; eb 02
    xor ax, ax                                ; 31 c0
    db  00fh, 0b7h, 0d0h
    ; movzx dx, ax                              ; 0f b7 d0
    db  00fh, 0b7h, 0c3h
    ; movzx ax, bx                              ; 0f b7 c3
    call 0da5fh                               ; e8 4b ff
    db  0ffh
    db  0ffh
    mov dx, 00cfch                            ; ba fc 0c
    add byte [bx+si], al                      ; 00 00
    in ax, DX                                 ; ed
    mov word [di-00ch], ax                    ; 89 45 f4
    mov word [di-014h], strict word 00000h    ; c7 45 ec 00 00
    add byte [bx+si], al                      ; 00 00
    test si, si                               ; 85 f6
    je short 0db30h                           ; 74 06
    shr ax, 008h                              ; c1 e8 08
    mov word [di-00ch], ax                    ; 89 45 f4
    mov ax, word [di-00ch]                    ; 8b 45 f4
    cmp ax, word [di-018h]                    ; 3b 45 e8
    je short 0db3eh                           ; 74 06
    cmp word [di-014h], strict byte 00000h    ; 83 7d ec 00
    je short 0db45h                           ; 74 07
    dec cx                                    ; 49
    cmp ecx, strict byte 0ffffffffh           ; 66 83 f9 ff
    je short 0db5dh                           ; 74 18
    add bx, di                                ; 01 fb
    db  00fh, 0b7h, 0c3h
    ; movzx ax, bx                              ; 0f b7 c3
    sar ax, 008h                              ; c1 f8 08
    mov word [di-010h], ax                    ; 89 45 f0
    movzx ax, byte [di-004h]                  ; 0f b6 45 fc
    cmp ax, word [di-010h]                    ; 3b 45 f0
    jnl near 0da92h                           ; 0f 8d 37 ff
    db  0ffh
    jmp word [bp-07dh]                        ; ff 66 83
    stc                                       ; f9
    push word [di+008h]                       ; ff 75 08
    db  00fh, 0b7h, 0c3h
    ; movzx ax, bx                              ; 0f b7 c3
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    retn                                      ; c3
    mov ax, strict word 0ffffh                ; b8 ff ff
    add byte [bx+si], al                      ; 00 00
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop cx                                    ; 59
    retn                                      ; c3
_pci32_function:                             ; 0xfdb75 LB 0x254
    push bx                                   ; 53
    push si                                   ; 56
    push di                                   ; 57
    enter 00008h, 000h                        ; c8 08 00 00
    and dword [di+030h], strict dword 0658100ffh ; 66 81 65 30 ff 00 81 65
    cmp dh, bh                                ; 38 fe
    inc word [bx+si]                          ; ff 00
    add byte [bp+di+03045h], cl               ; 00 8b 45 30
    xor ah, ah                                ; 30 e4
    cmp eax, strict dword 029720003h          ; 66 3d 03 00 72 29
    jbe near 0dc31h                           ; 0f 86 99 00
    add byte [bx+si], al                      ; 00 00
    cmp eax, strict dword 0840f000eh          ; 66 3d 0e 00 0f 84
    lodsw                                     ; ad
    add word [bx+si], ax                      ; 01 00
    add byte [bp+03dh], ah                    ; 00 66 3d
    or byte [bx+si], al                       ; 08 00
    jc near 0ddb2h                            ; 0f 82 06 02
    add byte [bx+si], al                      ; 00 00
    cmp eax, strict dword 0860f000dh          ; 66 3d 0d 00 0f 86
    test AL, strict byte 000h                 ; a8 00
    add byte [bx+si], al                      ; 00 00
    jmp near 0ddb2h                           ; e9 f7 01
    add byte [bx+si], al                      ; 00 00
    cmp eax, strict dword 028740002h          ; 66 3d 02 00 74 28
    cmp eax, strict dword 0850f0001h          ; 66 3d 01 00 0f 85
    out 001h, ax                              ; e7 01
    add byte [bx+si], al                      ; 00 00
    mov dword [di+030h], strict dword 0c7660001h ; 66 c7 45 30 01 00 66 c7
    inc bp                                    ; 45
    and AL, strict byte 010h                  ; 24 10
    add dh, byte [bx+di]                      ; 02 31
    sal byte [bp-077h], 045h                  ; c0 66 89 45
    sub AL, strict byte 0c7h                  ; 2c c7
    inc bp                                    ; 45
    sub byte [bx+si+043h], dl                 ; 28 50 43
    dec cx                                    ; 49
    and cl, cl                                ; 20 c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    cmp dword [di+028h], strict byte 0ffffffffh ; 66 83 7d 28 ff
    jne short 0dbffh                          ; 75 0d
    mov ax, word [di+030h]                    ; 8b 45 30
    xor ah, ah                                ; 30 e4
    or ah, 083h                               ; 80 cc 83
    jmp near 0ddbah                           ; e9 bd 01
    add byte [bx+si], al                      ; 00 00
    xor bx, bx                                ; 31 db
    db  00fh, 0b7h, 055h, 018h
    ; movzx dx, word [di+018h]                  ; 0f b7 55 18
    db  00fh, 0b7h, 045h, 02ch
    ; movzx ax, word [di+02ch]                  ; 0f b7 45 2c
    sal ax, 010h                              ; c1 e0 10
    db  00fh, 0b7h, 04dh, 028h
    ; movzx cx, word [di+028h]                  ; 0f b7 4d 28
    or ax, cx                                 ; 09 c8
    call 0da7eh                               ; e8 69 fe
    db  0ffh
    jmp word [bp+03dh]                        ; ff 66 3d
    db  0ffh
    push word [di+00dh]                       ; ff 75 0d
    mov ax, word [di+030h]                    ; 8b 45 30
    xor ah, ah                                ; 30 e4
    or ah, 086h                               ; 80 cc 86
    jmp near 0ddbah                           ; e9 92 01
    add byte [bx+si], al                      ; 00 00
    mov dword [di+024h], eax                  ; 66 89 45 24
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    db  00fh, 0b7h, 055h, 018h
    ; movzx dx, word [di+018h]                  ; 0f b7 55 18
    mov ax, word [di+02ch]                    ; 8b 45 2c
    mov bx, strict word 00001h                ; bb 01 00
    add byte [bx+si], al                      ; 00 00
    call 0da7eh                               ; e8 3c fe
    db  0ffh
    jmp word [bp+03dh]                        ; ff 66 3d
    db  0ffh
    push word [di+00dh]                       ; ff 75 0d
    mov ax, word [di+030h]                    ; 8b 45 30
    xor ah, ah                                ; 30 e4
    or ah, 086h                               ; 80 cc 86
    jmp near 0ddbah                           ; e9 65 01
    add byte [bx+si], al                      ; 00 00
    mov dword [di+024h], eax                  ; 66 89 45 24
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    cmp dword [di+014h], strict dword 00d720100h ; 66 81 7d 14 00 01 72 0d
    mov ax, word [di+030h]                    ; 8b 45 30
    xor ah, ah                                ; 30 e4
    or ah, 087h                               ; 80 cc 87
    jmp near 0ddbah                           ; e9 47 01
    add byte [bx+si], al                      ; 00 00
    db  00fh, 0b7h, 055h, 014h
    ; movzx dx, word [di+014h]                  ; 0f b7 55 14
    db  00fh, 0b7h, 045h, 024h
    ; movzx ax, word [di+024h]                  ; 0f b7 45 24
    call 0da5fh                               ; e8 df fd
    db  0ffh
    dec word [bp+di+03045h]                   ; ff 8b 45 30
    xor ah, ah                                ; 30 e4
    cmp eax, strict dword 02172000ah          ; 66 3d 0a 00 72 21
    jbe short 0dcffh                          ; 76 70
    cmp eax, strict dword 0840f000dh          ; 66 3d 0d 00 0f 84
    stosb                                     ; aa
    add byte [bx+si], al                      ; 00 00
    add byte [bp+03dh], ah                    ; 00 66 3d
    or AL, strict byte 000h                   ; 0c 00
    je near 0dd25h                            ; 0f 84 84 00
    add byte [bx+si], al                      ; 00 00
    cmp eax, strict dword 06474000bh          ; 66 3d 0b 00 74 64
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    cmp eax, strict dword 02e740009h          ; 66 3d 09 00 74 2e
    cmp eax, strict dword 005740008h          ; 66 3d 08 00 74 05
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    mov bx, word [di+02ch]                    ; 8b 5d 2c
    xor bl, bl                                ; 30 db
    mov ax, word [di+014h]                    ; 8b 45 14
    xor ah, ah                                ; 30 e4
    and AL, strict byte 003h                  ; 24 03
    db  00fh, 0b7h, 0d0h
    ; movzx dx, ax                              ; 0f b7 d0
    add dx, 00cfch                            ; 81 c2 fc 0c
    add byte [bx+si], al                      ; 00 00
    db  02bh, 0c0h
    ; sub ax, ax                                ; 2b c0
    in AL, DX                                 ; ec
    or bx, ax                                 ; 09 c3
    mov dword [di+02ch], ebx                  ; 66 89 5d 2c
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    mov ax, word [di+014h]                    ; 8b 45 14
    xor ah, ah                                ; 30 e4
    and AL, strict byte 002h                  ; 24 02
    db  00fh, 0b7h, 0d0h
    ; movzx dx, ax                              ; 0f b7 d0
    add dx, 00cfch                            ; 81 c2 fc 0c
    add byte [bx+si], al                      ; 00 00
    db  02bh, 0c0h
    ; sub ax, ax                                ; 2b c0
    in eax, DX                                ; 66 ed
    mov dword [di+02ch], eax                  ; 66 89 45 2c
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    mov dx, 00cfch                            ; ba fc 0c
    add byte [bx+si], al                      ; 00 00
    in ax, DX                                 ; ed
    mov word [di+02ch], ax                    ; 89 45 2c
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    mov ax, word [di+02ch]                    ; 8b 45 2c
    mov dx, word [di+014h]                    ; 8b 55 14
    xor dh, dh                                ; 30 f6
    and dl, 003h                              ; 80 e2 03
    db  00fh, 0b7h, 0d2h
    ; movzx dx, dx                              ; 0f b7 d2
    add dx, 00cfch                            ; 81 c2 fc 0c
    add byte [bx+si], al                      ; 00 00
    out DX, AL                                ; ee
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    db  00fh, 0b7h, 045h, 02ch
    ; movzx ax, word [di+02ch]                  ; 0f b7 45 2c
    mov dx, word [di+014h]                    ; 8b 55 14
    xor dh, dh                                ; 30 f6
    and dl, 002h                              ; 80 e2 02
    db  00fh, 0b7h, 0d2h
    ; movzx dx, dx                              ; 0f b7 d2
    add dx, 00cfch                            ; 81 c2 fc 0c
    add byte [bx+si], al                      ; 00 00
    out DX, eax                               ; 66 ef
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    mov ax, word [di+02ch]                    ; 8b 45 2c
    mov dx, 00cfch                            ; ba fc 0c
    add byte [bx+si], al                      ; 00 00
    out DX, ax                                ; ef
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    db  00fh, 0b7h, 045h, 014h
    ; movzx ax, word [di+014h]                  ; 0f b7 45 14
    mov es, [di+034h]                         ; 8e 45 34
    mov [di-004h], es                         ; 8c 45 fc
    mov bx, ax                                ; 89 c3
    mov edx, dword [di]                       ; 66 8b 15
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si], al                      ; 00 00
    db  066h, 026h, 03bh, 010h
    ; cmp edx, dword [es:bx+si]                 ; 66 26 3b 10
    jbe short 0dd7ch                          ; 76 12
    mov ax, word [di+030h]                    ; 8b 45 30
    xor ah, ah                                ; 30 e4
    or ah, 089h                               ; 80 cc 89
    mov dword [di+030h], eax                  ; 66 89 45 30
    or word [di+038h], strict byte 00001h     ; 83 4d 38 01
    jmp short 0dda2h                          ; eb 26
    db  00fh, 0b7h, 0cah
    ; movzx cx, dx                              ; 0f b7 ca
    db  066h, 026h, 08bh, 050h, 006h
    ; mov edx, dword [es:bx+si+006h]            ; 66 26 8b 50 06
    mov word [di-008h], dx                    ; 89 55 f8
    mov di, word [es:bx+si+002h]              ; 26 8b 78 02
    mov dx, ds                                ; 8c da
    mov si, 0f2c0h                            ; be c0 f2
    add byte [bx+si], al                      ; 00 00
    mov es, [di-008h]                         ; 8e 45 f8
    push DS                                   ; 1e
    db  066h, 08eh, 0dah
    ; mov ds, edx                               ; 66 8e da
    rep movsb                                 ; f3 a4
    pop DS                                    ; 1f
    mov dword [di+024h], strict dword 0a1660a00h ; 66 c7 45 24 00 0a 66 a1
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si], al                      ; 00 00
    mov es, [di-004h]                         ; 8e 45 fc
    db  066h, 026h, 089h, 003h
    ; mov dword [es:bp+di], eax                 ; 66 26 89 03
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3
    mov ax, word [di+030h]                    ; 8b 45 30
    xor ah, ah                                ; 30 e4
    or ah, 081h                               ; 80 cc 81
    mov dword [di+030h], eax                  ; 66 89 45 30
    or word [di+038h], strict byte 00001h     ; 83 4d 38 01
    leave                                     ; c9
    pop di                                    ; 5f
    pop si                                    ; 5e
    pop bx                                    ; 5b
    retn                                      ; c3

  ; Padding 0x3 bytes at 0xfddc9
  times 3 db 0

section BIOS32CONST progbits vstart=0xddcc align=1 ; size=0x0 class=FAR_DATA group=BIOS32_GROUP

section BIOS32CONST2 progbits vstart=0xddcc align=1 ; size=0x0 class=FAR_DATA group=BIOS32_GROUP

section BIOS32_DATA progbits vstart=0xddcc align=1 ; size=0x0 class=FAR_DATA group=BIOS32_GROUP

  ; Padding 0x234 bytes at 0xfddcc
  times 564 db 0

section BIOSSEG progbits vstart=0xe000 align=1 ; size=0x2000 class=CODE group=AUTO
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 058h, 04dh
eoi_jmp_post:                                ; 0xfe030 LB 0xb
    call 0e03bh                               ; e8 08 00
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    jmp far [00467h]                          ; ff 2e 67 04
eoi_both_pics:                               ; 0xfe03b LB 0x4
    mov AL, strict byte 020h                  ; b0 20
    out strict byte 0a0h, AL                  ; e6 a0
eoi_master_pic:                              ; 0xfe03f LB 0x1c
    mov AL, strict byte 020h                  ; b0 20
    out strict byte 020h, AL                  ; e6 20
    retn                                      ; c3
    times 0x15 db 0
    db  'XM'
post:                                        ; 0xfe05b LB 0x32
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    out strict byte 00dh, AL                  ; e6 0d
    out strict byte 0dah, AL                  ; e6 da
    mov AL, strict byte 0c0h                  ; b0 c0
    out strict byte 0d6h, AL                  ; e6 d6
    mov AL, strict byte 000h                  ; b0 00
    out strict byte 0d4h, AL                  ; e6 d4
    mov AL, strict byte 00fh                  ; b0 0f
    out strict byte 070h, AL                  ; e6 70
    in AL, strict byte 071h                   ; e4 71
    db  08ah, 0d8h
    ; mov bl, al                                ; 8a d8
    mov AL, strict byte 00fh                  ; b0 0f
    out strict byte 070h, AL                  ; e6 70
    mov AL, strict byte 000h                  ; b0 00
    out strict byte 071h, AL                  ; e6 71
    db  08ah, 0c3h
    ; mov al, bl                                ; 8a c3
    cmp AL, strict byte 000h                  ; 3c 00
    je short 0e098h                           ; 74 19
    cmp AL, strict byte 00dh                  ; 3c 0d
    jnc short 0e098h                          ; 73 15
    cmp AL, strict byte 009h                  ; 3c 09
    je short 0e098h                           ; 74 11
    cmp AL, strict byte 005h                  ; 3c 05
    je short 0e030h                           ; 74 a5
    jmp short 0e098h                          ; eb 0b
set_int_vects:                               ; 0xfe08d LB 0xb
    mov word [bx], ax                         ; 89 07
    mov word [bx+002h], dx                    ; 89 57 02
    add bx, strict byte 00004h                ; 83 c3 04
    loop 0e08dh                               ; e2 f6
    retn                                      ; c3
normal_post:                                 ; 0xfe098 LB 0x22b
    cli                                       ; fa
    mov ax, 07800h                            ; b8 00 78
    db  08bh, 0e0h
    ; mov sp, ax                                ; 8b e0
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov ss, ax                                ; 8e d0
    mov es, ax                                ; 8e c0
    db  033h, 0ffh
    ; xor di, di                                ; 33 ff
    cld                                       ; fc
    mov cx, 00239h                            ; b9 39 02
    rep stosw                                 ; f3 ab
    inc di                                    ; 47
    inc di                                    ; 47
    mov cx, 005c6h                            ; b9 c6 05
    rep stosw                                 ; f3 ab
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    add bx, 01000h                            ; 81 c3 00 10
    cmp bx, 09000h                            ; 81 fb 00 90
    jnc short 0e0cch                          ; 73 0b
    mov es, bx                                ; 8e c3
    db  033h, 0ffh
    ; xor di, di                                ; 33 ff
    mov cx, 08000h                            ; b9 00 80
    rep stosw                                 ; f3 ab
    jmp short 0e0b7h                          ; eb eb
    mov es, bx                                ; 8e c3
    db  033h, 0ffh
    ; xor di, di                                ; 33 ff
    mov cx, 07ff8h                            ; b9 f8 7f
    rep stosw                                 ; f3 ab
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 01712h                               ; e8 35 36
    call 0e8e0h                               ; e8 00 08
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    mov ds, bx                                ; 8e db
    mov cx, strict word 00060h                ; b9 60 00
    mov ax, 0ff53h                            ; b8 53 ff
    mov dx, 0f000h                            ; ba 00 f0
    call 0e08dh                               ; e8 9d ff
    mov bx, 001a0h                            ; bb a0 01
    mov cx, strict word 00010h                ; b9 10 00
    call 0e08dh                               ; e8 94 ff
    mov ax, 0027fh                            ; b8 7f 02
    mov word [00413h], ax                     ; a3 13 04
    mov ax, 0f84dh                            ; b8 4d f8
    mov word [00044h], ax                     ; a3 44 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00046h], ax                     ; a3 46 00
    mov ax, 0f841h                            ; b8 41 f8
    mov word [00048h], ax                     ; a3 48 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0004ah], ax                     ; a3 4a 00
    mov ax, 0f859h                            ; b8 59 f8
    mov word [00054h], ax                     ; a3 54 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00056h], ax                     ; a3 56 00
    mov ax, 0efd4h                            ; b8 d4 ef
    mov word [0005ch], ax                     ; a3 5c 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0005eh], ax                     ; a3 5e 00
    mov ax, 0f0a4h                            ; b8 a4 f0
    mov word [00060h], ax                     ; a3 60 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00062h], ax                     ; a3 62 00
    mov ax, 0e6f2h                            ; b8 f2 e6
    mov word [00064h], ax                     ; a3 64 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00066h], ax                     ; a3 66 00
    mov ax, 0efedh                            ; b8 ed ef
    mov word [00070h], ax                     ; a3 70 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00072h], ax                     ; a3 72 00
    call 0e7c0h                               ; e8 6a 06
    mov ax, 0fea5h                            ; b8 a5 fe
    mov word [00020h], ax                     ; a3 20 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00022h], ax                     ; a3 22 00
    mov AL, strict byte 034h                  ; b0 34
    out strict byte 043h, AL                  ; e6 43
    mov AL, strict byte 000h                  ; b0 00
    out strict byte 040h, AL                  ; e6 40
    out strict byte 040h, AL                  ; e6 40
    mov ax, 0e987h                            ; b8 87 e9
    mov word [00024h], ax                     ; a3 24 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00026h], ax                     ; a3 26 00
    mov ax, 0e82eh                            ; b8 2e e8
    mov word [00058h], ax                     ; a3 58 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0005ah], ax                     ; a3 5a 00
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov byte [00417h], AL                     ; a2 17 04
    mov byte [00418h], AL                     ; a2 18 04
    mov byte [00419h], AL                     ; a2 19 04
    mov byte [00471h], AL                     ; a2 71 04
    mov byte [00497h], AL                     ; a2 97 04
    mov AL, strict byte 010h                  ; b0 10
    mov byte [00496h], AL                     ; a2 96 04
    mov bx, strict word 0001eh                ; bb 1e 00
    mov word [0041ah], bx                     ; 89 1e 1a 04
    mov word [0041ch], bx                     ; 89 1e 1c 04
    mov word [00480h], bx                     ; 89 1e 80 04
    mov bx, strict word 0003eh                ; bb 3e 00
    mov word [00482h], bx                     ; 89 1e 82 04
    push DS                                   ; 1e
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 04939h                               ; e8 80 67
    pop DS                                    ; 1f
    mov AL, strict byte 014h                  ; b0 14
    out strict byte 070h, AL                  ; e6 70
    in AL, strict byte 071h                   ; e4 71
    mov byte [00410h], AL                     ; a2 10 04
    mov ax, 0ff53h                            ; b8 53 ff
    mov word [0003ch], ax                     ; a3 3c 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0003eh], ax                     ; a3 3e 00
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    mov CL, strict byte 014h                  ; b1 14
    mov dx, 00378h                            ; ba 78 03
    call 0ecedh                               ; e8 10 0b
    mov dx, 00278h                            ; ba 78 02
    call 0ecedh                               ; e8 0a 0b
    sal bx, 00eh                              ; c1 e3 0e
    mov ax, word [00410h]                     ; a1 10 04
    and ax, 03fffh                            ; 25 ff 3f
    db  00bh, 0c3h
    ; or ax, bx                                 ; 0b c3
    mov word [00410h], ax                     ; a3 10 04
    mov ax, 0e746h                            ; b8 46 e7
    mov word [0002ch], ax                     ; a3 2c 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0002eh], ax                     ; a3 2e 00
    mov ax, 0e746h                            ; b8 46 e7
    mov word [00030h], ax                     ; a3 30 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00032h], ax                     ; a3 32 00
    mov ax, 0e739h                            ; b8 39 e7
    mov word [00050h], ax                     ; a3 50 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00052h], ax                     ; a3 52 00
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    mov CL, strict byte 00ah                  ; b1 0a
    mov dx, 003f8h                            ; ba f8 03
    call 0ed0bh                               ; e8 ec 0a
    mov dx, 002f8h                            ; ba f8 02
    call 0ed0bh                               ; e8 e6 0a
    mov dx, 003e8h                            ; ba e8 03
    call 0ed0bh                               ; e8 e0 0a
    mov dx, 002e8h                            ; ba e8 02
    call 0ed0bh                               ; e8 da 0a
    sal bx, 009h                              ; c1 e3 09
    mov ax, word [00410h]                     ; a1 10 04
    and ax, 0f1ffh                            ; 25 ff f1
    db  00bh, 0c3h
    ; or ax, bx                                 ; 0b c3
    mov word [00410h], ax                     ; a3 10 04
    mov ax, 0fe6eh                            ; b8 6e fe
    mov word [00068h], ax                     ; a3 68 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0006ah], ax                     ; a3 6a 00
    mov ax, 0ff53h                            ; b8 53 ff
    mov word [00128h], ax                     ; a3 28 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0012ah], ax                     ; a3 2a 01
    mov ax, 0fe8fh                            ; b8 8f fe
    mov word [001c0h], ax                     ; a3 c0 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [001c2h], ax                     ; a3 c2 01
    call 0edbfh                               ; e8 59 0b
    mov ax, 0f8a4h                            ; b8 a4 f8
    mov word [001d0h], ax                     ; a3 d0 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [001d2h], ax                     ; a3 d2 01
    mov ax, 0e2cah                            ; b8 ca e2
    mov word [001d4h], ax                     ; a3 d4 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [001d6h], ax                     ; a3 d6 01
    mov ax, 0f065h                            ; b8 65 f0
    mov word [00040h], ax                     ; a3 40 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00042h], ax                     ; a3 42 00
    call 0e79bh                               ; e8 0e 05
    call 0f13ch                               ; e8 ac 0e
    call 0f1c1h                               ; e8 2e 0f
    call 0e758h                               ; e8 c2 04
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 01b48h                               ; e8 ac 38
    call 01f52h                               ; e8 b3 3c
    call 08746h                               ; e8 a4 a4
    call 07825h                               ; e8 80 95
    call 0ed2fh                               ; e8 87 0a
    call 0e2d2h                               ; e8 27 00
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 01732h                               ; e8 81 34
    call 035f4h                               ; e8 40 53
    sti                                       ; fb
    int 019h                                  ; cd 19
    sti                                       ; fb
    hlt                                       ; f4
    jmp short 0e2b8h                          ; eb fd
    cli                                       ; fa
    hlt                                       ; f4
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si], al                      ; 00 00
    pop ax                                    ; 58
    dec bp                                    ; 4d
nmi:                                         ; 0xfe2c3 LB 0x7
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 016eeh                               ; e8 25 34
    iret                                      ; cf
int75_handler:                               ; 0xfe2ca LB 0x8
    out strict byte 0f0h, AL                  ; e6 f0
    call 0e03bh                               ; e8 6c fd
    int 002h                                  ; cd 02
    iret                                      ; cf
hard_drive_post:                             ; 0xfe2d2 LB 0x12c
    mov AL, strict byte 00ah                  ; b0 0a
    mov dx, 003f6h                            ; ba f6 03
    out DX, AL                                ; ee
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov byte [00474h], AL                     ; a2 74 04
    mov byte [00477h], AL                     ; a2 77 04
    mov byte [0048ch], AL                     ; a2 8c 04
    mov byte [0048dh], AL                     ; a2 8d 04
    mov byte [0048eh], AL                     ; a2 8e 04
    mov AL, strict byte 0c0h                  ; b0 c0
    mov byte [00476h], AL                     ; a2 76 04
    mov ax, 0e3feh                            ; b8 fe e3
    mov word [0004ch], ax                     ; a3 4c 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0004eh], ax                     ; a3 4e 00
    mov ax, 0f8d2h                            ; b8 d2 f8
    mov word [001d8h], ax                     ; a3 d8 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [001dah], ax                     ; a3 da 01
    mov ax, strict word 0003dh                ; b8 3d 00
    mov word [00104h], ax                     ; a3 04 01
    mov ax, 09fc0h                            ; b8 c0 9f
    mov word [00106h], ax                     ; a3 06 01
    mov ax, strict word 0004dh                ; b8 4d 00
    mov word [00118h], ax                     ; a3 18 01
    mov ax, 09fc0h                            ; b8 c0 9f
    mov word [0011ah], ax                     ; a3 1a 01
    retn                                      ; c3
    times 0xdb db 0
    db  'XM'
int13_handler:                               ; 0xfe3fe LB 0x3
    jmp near 0ec5bh                           ; e9 5a 08
rom_fdpt:                                    ; 0xfe401 LB 0x2f1
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 058h
    db  04dh
int19_handler:                               ; 0xfe6f2 LB 0x61
    jmp near 0f0ach                           ; e9 b7 09
    or word [bx+si], ax                       ; 09 00
    cld                                       ; fc
    add byte [bx+di], al                      ; 00 01
    je short 0e73ch                           ; 74 40
    times 0x2b db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    times 0xe db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    push DS                                   ; 1e
    push ES                                   ; 06
    pushaw                                    ; 60
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 05c5fh                               ; e8 1d 75
    popaw                                     ; 61
    pop ES                                    ; 07
    pop DS                                    ; 1f
    iret                                      ; cf
    push DS                                   ; 1e
    push ES                                   ; 06
    pushaw                                    ; 60
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 01683h                               ; e8 34 2f
    popaw                                     ; 61
    pop ES                                    ; 07
    pop DS                                    ; 1f
    iret                                      ; cf
rom_checksum:                                ; 0xfe753 LB 0x5
    push ax                                   ; 50
    db  032h, 0c0h
    ; xor al, al                                ; 32 c0
    pop ax                                    ; 58
    retn                                      ; c3
rom_scan:                                    ; 0xfe758 LB 0x43
    mov cx, 0c000h                            ; b9 00 c0
    mov ds, cx                                ; 8e d9
    mov ax, strict word 00004h                ; b8 04 00
    cmp word [word 00000h], 0aa55h            ; 81 3e 00 00 55 aa
    jne short 0e78bh                          ; 75 23
    call 0e753h                               ; e8 e8 ff
    jne short 0e78bh                          ; 75 1e
    mov AL, byte [00002h]                     ; a0 02 00
    test AL, strict byte 003h                 ; a8 03
    je short 0e778h                           ; 74 04
    and AL, strict byte 0fch                  ; 24 fc
    add AL, strict byte 004h                  ; 04 04
    db  033h, 0dbh
    ; xor bx, bx                                ; 33 db
    mov ds, bx                                ; 8e db
    push ax                                   ; 50
    push cx                                   ; 51
    push strict byte 00003h                   ; 6a 03
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    call far [byte bp+000h]                   ; ff 5e 00
    cli                                       ; fa
    add sp, strict byte 00002h                ; 83 c4 02
    pop cx                                    ; 59
    pop ax                                    ; 58
    sal ax, 005h                              ; c1 e0 05
    db  003h, 0c8h
    ; add cx, ax                                ; 03 c8
    cmp cx, 0e800h                            ; 81 f9 00 e8
    jbe short 0e75bh                          ; 76 c5
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    retn                                      ; c3
init_pic:                                    ; 0xfe79b LB 0x25
    mov AL, strict byte 011h                  ; b0 11
    out strict byte 020h, AL                  ; e6 20
    out strict byte 0a0h, AL                  ; e6 a0
    mov AL, strict byte 008h                  ; b0 08
    out strict byte 021h, AL                  ; e6 21
    mov AL, strict byte 070h                  ; b0 70
    out strict byte 0a1h, AL                  ; e6 a1
    mov AL, strict byte 004h                  ; b0 04
    out strict byte 021h, AL                  ; e6 21
    mov AL, strict byte 002h                  ; b0 02
    out strict byte 0a1h, AL                  ; e6 a1
    mov AL, strict byte 001h                  ; b0 01
    out strict byte 021h, AL                  ; e6 21
    out strict byte 0a1h, AL                  ; e6 a1
    mov AL, strict byte 0b8h                  ; b0 b8
    out strict byte 021h, AL                  ; e6 21
    mov AL, strict byte 08fh                  ; b0 8f
    out strict byte 0a1h, AL                  ; e6 a1
    retn                                      ; c3
ebda_post:                                   ; 0xfe7c0 LB 0xa4
    mov ax, 0e746h                            ; b8 46 e7
    mov word [00034h], ax                     ; a3 34 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00036h], ax                     ; a3 36 00
    mov ax, 0e746h                            ; b8 46 e7
    mov word [0003ch], ax                     ; a3 3c 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0003eh], ax                     ; a3 3e 00
    mov ax, 0e746h                            ; b8 46 e7
    mov word [001c8h], ax                     ; a3 c8 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [001cah], ax                     ; a3 ca 01
    mov ax, 0e746h                            ; b8 46 e7
    mov word [001dch], ax                     ; a3 dc 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [001deh], ax                     ; a3 de 01
    mov ax, 09fc0h                            ; b8 c0 9f
    mov ds, ax                                ; 8e d8
    mov byte [word 00000h], 001h              ; c6 06 00 00 01
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov word [0040eh], 09fc0h                 ; c7 06 0e 04 c0 9f
    retn                                      ; c3
    times 0x27 db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    sti                                       ; fb
    push ES                                   ; 06
    push DS                                   ; 1e
    pushaw                                    ; 60
    cmp ah, 000h                              ; 80 fc 00
    je short 0e846h                           ; 74 0f
    cmp ah, 010h                              ; 80 fc 10
    je short 0e846h                           ; 74 0a
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 05039h                               ; e8 f7 67
    popaw                                     ; 61
    pop DS                                    ; 1f
    pop ES                                    ; 07
    iret                                      ; cf
    mov bx, strict word 00040h                ; bb 40 00
    mov ds, bx                                ; 8e db
    cli                                       ; fa
    mov bx, word [word 0001ah]                ; 8b 1e 1a 00
    cmp bx, word [word 0001ch]                ; 3b 1e 1c 00
    jne short 0e85ah                          ; 75 04
    sti                                       ; fb
    nop                                       ; 90
    jmp short 0e84bh                          ; eb f1
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 05039h                               ; e8 d9 67
    popaw                                     ; 61
    pop DS                                    ; 1f
    pop ES                                    ; 07
    iret                                      ; cf
pmode_enter:                                 ; 0xfe864 LB 0x1b
    push CS                                   ; 0e
    pop DS                                    ; 1f
    lgdt [cs:0e892h]                          ; 2e 0f 01 16 92 e8
    mov eax, cr0                              ; 0f 20 c0
    or AL, strict byte 001h                   ; 0c 01
    mov cr0, eax                              ; 0f 22 c0
    jmp far 00020h:0e879h                     ; ea 79 e8 20 00
    mov ax, strict word 00018h                ; b8 18 00
    mov ds, ax                                ; 8e d8
    retn                                      ; c3
pmode_exit:                                  ; 0xfe87f LB 0x13
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov eax, cr0                              ; 0f 20 c0
    and AL, strict byte 0feh                  ; 24 fe
    mov cr0, eax                              ; 0f 22 c0
    jmp far 0f000h:0e891h                     ; ea 91 e8 00 f0
    retn                                      ; c3
pmbios_gdt_desc:                             ; 0xfe892 LB 0x6
    db  047h, 000h, 098h, 0e8h, 00fh, 000h
pmbios_gdt:                                  ; 0xfe898 LB 0x48
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  0ffh, 0ffh, 000h, 000h, 000h, 09bh, 0cfh, 000h, 0ffh, 0ffh, 000h, 000h, 000h, 093h, 0cfh, 000h
    db  0ffh, 0ffh, 000h, 000h, 00fh, 09bh, 000h, 000h, 0ffh, 0ffh, 000h, 000h, 000h, 093h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  0ffh, 0ffh, 000h, 004h, 000h, 093h, 000h, 000h
pmode_setup:                                 ; 0xfe8e0 LB 0x37b
    push eax                                  ; 66 50
    push esi                                  ; 66 56
    pushfw                                    ; 9c
    cli                                       ; fa
    call 0e864h                               ; e8 7b ff
    mov eax, cr0                              ; 0f 20 c0
    and eax, strict dword 09fffffffh          ; 66 25 ff ff ff 9f
    mov cr0, eax                              ; 0f 22 c0
    mov esi, strict dword 0fee00350h          ; 66 be 50 03 e0 fe
    mov eax, dword [esi]                      ; 67 66 8b 06
    and eax, strict dword 0fffe00ffh          ; 66 25 ff 00 fe ff
    or ah, 007h                               ; 80 cc 07
    mov dword [esi], eax                      ; 67 66 89 06
    mov esi, strict dword 0fee00360h          ; 66 be 60 03 e0 fe
    mov eax, dword [esi]                      ; 67 66 8b 06
    and eax, strict dword 0fffe00ffh          ; 66 25 ff 00 fe ff
    or ah, 004h                               ; 80 cc 04
    mov dword [esi], eax                      ; 67 66 89 06
    call 0e87fh                               ; e8 59 ff
    popfw                                     ; 9d
    pop esi                                   ; 66 5e
    pop eax                                   ; 66 58
    retn                                      ; c3
    times 0x59 db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    cli                                       ; fa
    push ax                                   ; 50
    mov AL, strict byte 0adh                  ; b0 ad
    out strict byte 064h, AL                  ; e6 64
    mov AL, strict byte 00bh                  ; b0 0b
    out strict byte 020h, AL                  ; e6 20
    in AL, strict byte 020h                   ; e4 20
    and AL, strict byte 002h                  ; 24 02
    je short 0e9d6h                           ; 74 3f
    in AL, strict byte 060h                   ; e4 60
    push DS                                   ; 1e
    pushaw                                    ; 60
    cld                                       ; fc
    mov AH, strict byte 04fh                  ; b4 4f
    stc                                       ; f9
    int 015h                                  ; cd 15
    jnc short 0e9d0h                          ; 73 2d
    sti                                       ; fb
    cmp AL, strict byte 0e0h                  ; 3c e0
    jne short 0e9b6h                          ; 75 0e
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov AL, byte [00496h]                     ; a0 96 04
    or AL, strict byte 002h                   ; 0c 02
    mov byte [00496h], AL                     ; a2 96 04
    jmp short 0e9d0h                          ; eb 1a
    cmp AL, strict byte 0e1h                  ; 3c e1
    jne short 0e9c8h                          ; 75 0e
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov AL, byte [00496h]                     ; a0 96 04
    or AL, strict byte 001h                   ; 0c 01
    mov byte [00496h], AL                     ; a2 96 04
    jmp short 0e9d0h                          ; eb 08
    push ES                                   ; 06
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 04c48h                               ; e8 79 62
    pop ES                                    ; 07
    popaw                                     ; 61
    pop DS                                    ; 1f
    cli                                       ; fa
    call 0e03fh                               ; e8 69 f6
    mov AL, strict byte 0aeh                  ; b0 ae
    out strict byte 064h, AL                  ; e6 64
    pop ax                                    ; 58
    iret                                      ; cf
    times 0x27b db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    jmp short 0ecb0h                          ; eb 55
int13_relocated:                             ; 0xfec5b LB 0x55
    cmp ah, 04ah                              ; 80 fc 4a
    jc short 0ec71h                           ; 72 11
    cmp ah, 04dh                              ; 80 fc 4d
    jnbe short 0ec71h                         ; 77 0c
    pushaw                                    ; 60
    push ES                                   ; 06
    push DS                                   ; 1e
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    push 0ece9h                               ; 68 e9 ec
    jmp near 03638h                           ; e9 c7 49
    push ES                                   ; 06
    push ax                                   ; 50
    push bx                                   ; 53
    push cx                                   ; 51
    push dx                                   ; 52
    call 0360ch                               ; e8 93 49
    cmp AL, strict byte 000h                  ; 3c 00
    je short 0ecabh                           ; 74 2e
    call 03622h                               ; e8 a2 49
    pop dx                                    ; 5a
    push dx                                   ; 52
    db  03ah, 0c2h
    ; cmp al, dl                                ; 3a c2
    jne short 0ec97h                          ; 75 11
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    pop ES                                    ; 07
    pushaw                                    ; 60
    push ES                                   ; 06
    push DS                                   ; 1e
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    push 0ece9h                               ; 68 e9 ec
    jmp near 03c2bh                           ; e9 94 4f
    and dl, 0e0h                              ; 80 e2 e0
    db  03ah, 0c2h
    ; cmp al, dl                                ; 3a c2
    jne short 0ecabh                          ; 75 0d
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    pop ES                                    ; 07
    push ax                                   ; 50
    push cx                                   ; 51
    push dx                                   ; 52
    push bx                                   ; 53
    db  0feh, 0cah
    ; dec dl                                    ; fe ca
    jmp short 0ecb4h                          ; eb 09
    pop dx                                    ; 5a
    pop cx                                    ; 59
    pop bx                                    ; 5b
    pop ax                                    ; 58
    pop ES                                    ; 07
int13_noeltorito:                            ; 0xfecb0 LB 0x4
    push ax                                   ; 50
    push cx                                   ; 51
    push dx                                   ; 52
    push bx                                   ; 53
int13_legacy:                                ; 0xfecb4 LB 0x14
    push dx                                   ; 52
    push bp                                   ; 55
    push si                                   ; 56
    push di                                   ; 57
    push ES                                   ; 06
    push DS                                   ; 1e
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    test dl, 080h                             ; f6 c2 80
    jne short 0ecc8h                          ; 75 06
    push 0ece9h                               ; 68 e9 ec
    jmp near 02e56h                           ; e9 8e 41
int13_notfloppy:                             ; 0xfecc8 LB 0x14
    cmp dl, 0e0h                              ; 80 fa e0
    jc short 0ecdch                           ; 72 0f
    shr ebx, 010h                             ; 66 c1 eb 10
    push bx                                   ; 53
    call 04059h                               ; e8 84 53
    pop bx                                    ; 5b
    sal ebx, 010h                             ; 66 c1 e3 10
    jmp short 0ece9h                          ; eb 0d
int13_disk:                                  ; 0xfecdc LB 0xd
    cmp ah, 040h                              ; 80 fc 40
    jnbe short 0ece6h                         ; 77 05
    call 05352h                               ; e8 6e 66
    jmp short 0ece9h                          ; eb 03
    call 05793h                               ; e8 aa 6a
int13_out:                                   ; 0xfece9 LB 0x4
    pop DS                                    ; 1f
    pop ES                                    ; 07
    popaw                                     ; 61
    iret                                      ; cf
detect_parport:                              ; 0xfeced LB 0x1e
    push dx                                   ; 52
    inc dx                                    ; 42
    inc dx                                    ; 42
    in AL, DX                                 ; ec
    and AL, strict byte 0dfh                  ; 24 df
    out DX, AL                                ; ee
    pop dx                                    ; 5a
    mov AL, strict byte 0aah                  ; b0 aa
    out DX, AL                                ; ee
    in AL, DX                                 ; ec
    cmp AL, strict byte 0aah                  ; 3c aa
    jne short 0ed0ah                          ; 75 0d
    push bx                                   ; 53
    sal bx, 1                                 ; d1 e3
    mov word [bx+00408h], dx                  ; 89 97 08 04
    pop bx                                    ; 5b
    mov byte [bx+00478h], cl                  ; 88 8f 78 04
    inc bx                                    ; 43
    retn                                      ; c3
detect_serial:                               ; 0xfed0b LB 0x24
    push dx                                   ; 52
    inc dx                                    ; 42
    mov AL, strict byte 002h                  ; b0 02
    out DX, AL                                ; ee
    in AL, DX                                 ; ec
    cmp AL, strict byte 002h                  ; 3c 02
    jne short 0ed2dh                          ; 75 18
    inc dx                                    ; 42
    in AL, DX                                 ; ec
    cmp AL, strict byte 002h                  ; 3c 02
    jne short 0ed2dh                          ; 75 12
    dec dx                                    ; 4a
    db  032h, 0c0h
    ; xor al, al                                ; 32 c0
    pop dx                                    ; 5a
    push bx                                   ; 53
    sal bx, 1                                 ; d1 e3
    mov word [bx+00400h], dx                  ; 89 97 00 04
    pop bx                                    ; 5b
    mov byte [bx+0047ch], cl                  ; 88 8f 7c 04
    inc bx                                    ; 43
    retn                                      ; c3
    pop dx                                    ; 5a
    retn                                      ; c3
floppy_post:                                 ; 0xfed2f LB 0x87
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov AL, strict byte 000h                  ; b0 00
    mov byte [0043eh], AL                     ; a2 3e 04
    mov byte [0043fh], AL                     ; a2 3f 04
    mov byte [00440h], AL                     ; a2 40 04
    mov byte [00441h], AL                     ; a2 41 04
    mov byte [00442h], AL                     ; a2 42 04
    mov byte [00443h], AL                     ; a2 43 04
    mov byte [00444h], AL                     ; a2 44 04
    mov byte [00445h], AL                     ; a2 45 04
    mov byte [00446h], AL                     ; a2 46 04
    mov byte [00447h], AL                     ; a2 47 04
    mov byte [00448h], AL                     ; a2 48 04
    mov byte [0048bh], AL                     ; a2 8b 04
    mov AL, strict byte 010h                  ; b0 10
    out strict byte 070h, AL                  ; e6 70
    in AL, strict byte 071h                   ; e4 71
    db  08ah, 0e0h
    ; mov ah, al                                ; 8a e0
    shr al, 004h                              ; c0 e8 04
    je short 0ed6ah                           ; 74 04
    mov BL, strict byte 007h                  ; b3 07
    jmp short 0ed6ch                          ; eb 02
    mov BL, strict byte 000h                  ; b3 00
    db  08ah, 0c4h
    ; mov al, ah                                ; 8a c4
    and AL, strict byte 00fh                  ; 24 0f
    je short 0ed75h                           ; 74 03
    or bl, 070h                               ; 80 cb 70
    mov byte [0048fh], bl                     ; 88 1e 8f 04
    mov AL, strict byte 000h                  ; b0 00
    mov byte [00490h], AL                     ; a2 90 04
    mov byte [00491h], AL                     ; a2 91 04
    mov byte [00492h], AL                     ; a2 92 04
    mov byte [00493h], AL                     ; a2 93 04
    mov byte [00494h], AL                     ; a2 94 04
    mov byte [00495h], AL                     ; a2 95 04
    mov AL, strict byte 002h                  ; b0 02
    out strict byte 00ah, AL                  ; e6 0a
    mov ax, 0efc7h                            ; b8 c7 ef
    mov word [00078h], ax                     ; a3 78 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0007ah], ax                     ; a3 7a 00
    mov ax, 0ec59h                            ; b8 59 ec
    mov word [00100h], ax                     ; a3 00 01
    mov ax, 0f000h                            ; b8 00 f0
    mov word [00102h], ax                     ; a3 02 01
    mov ax, 0ef57h                            ; b8 57 ef
    mov word [00038h], ax                     ; a3 38 00
    mov ax, 0f000h                            ; b8 00 f0
    mov word [0003ah], ax                     ; a3 3a 00
    retn                                      ; c3
bcd_to_bin:                                  ; 0xfedb6 LB 0x9
    sal ax, 004h                              ; c1 e0 04
    shr al, 004h                              ; c0 e8 04
    aad 00ah                                  ; d5 0a
    retn                                      ; c3
rtc_post:                                    ; 0xfedbf LB 0x198
    db  066h, 033h, 0c0h
    ; xor eax, eax                              ; 66 33 c0
    mov AL, strict byte 000h                  ; b0 00
    out strict byte 070h, AL                  ; e6 70
    in AL, strict byte 071h                   ; e4 71
    call 0edb6h                               ; e8 eb ff
    mov edx, strict dword 00115cf2bh          ; 66 ba 2b cf 15 01
    mul edx                                   ; 66 f7 e2
    mov ebx, strict dword 0000f4240h          ; 66 bb 40 42 0f 00
    db  066h, 033h, 0d2h
    ; xor edx, edx                              ; 66 33 d2
    div ebx                                   ; 66 f7 f3
    db  066h, 08bh, 0c8h
    ; mov ecx, eax                              ; 66 8b c8
    db  066h, 033h, 0c0h
    ; xor eax, eax                              ; 66 33 c0
    mov AL, strict byte 002h                  ; b0 02
    out strict byte 070h, AL                  ; e6 70
    in AL, strict byte 071h                   ; e4 71
    call 0edb6h                               ; e8 c7 ff
    mov edx, strict dword 000a6af80h          ; 66 ba 80 af a6 00
    mul edx                                   ; 66 f7 e2
    mov ebx, strict dword 000002710h          ; 66 bb 10 27 00 00
    db  066h, 033h, 0d2h
    ; xor edx, edx                              ; 66 33 d2
    div ebx                                   ; 66 f7 f3
    db  066h, 003h, 0c8h
    ; add ecx, eax                              ; 66 03 c8
    db  066h, 033h, 0c0h
    ; xor eax, eax                              ; 66 33 c0
    mov AL, strict byte 004h                  ; b0 04
    out strict byte 070h, AL                  ; e6 70
    in AL, strict byte 071h                   ; e4 71
    call 0edb6h                               ; e8 a3 ff
    mov edx, strict dword 003e81d03h          ; 66 ba 03 1d e8 03
    mul edx                                   ; 66 f7 e2
    mov ebx, strict dword 0000003e8h          ; 66 bb e8 03 00 00
    db  066h, 033h, 0d2h
    ; xor edx, edx                              ; 66 33 d2
    div ebx                                   ; 66 f7 f3
    db  066h, 003h, 0c8h
    ; add ecx, eax                              ; 66 03 c8
    mov dword [0046ch], ecx                   ; 66 89 0e 6c 04
    db  032h, 0c0h
    ; xor al, al                                ; 32 c0
    mov byte [00470h], AL                     ; a2 70 04
    retn                                      ; c3
    times 0x11f db 0
    db  'XM'
int0e_handler:                               ; 0xfef57 LB 0x70
    push ax                                   ; 50
    push dx                                   ; 52
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 0c0h                  ; 3c c0
    je short 0ef81h                           ; 74 1e
    mov dx, 003f5h                            ; ba f5 03
    mov AL, strict byte 008h                  ; b0 08
    out DX, AL                                ; ee
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 0c0h                  ; 3c c0
    jne short 0ef69h                          ; 75 f6
    mov dx, 003f5h                            ; ba f5 03
    in AL, DX                                 ; ec
    mov dx, 003f4h                            ; ba f4 03
    in AL, DX                                 ; ec
    and AL, strict byte 0c0h                  ; 24 c0
    cmp AL, strict byte 0c0h                  ; 3c c0
    je short 0ef73h                           ; 74 f2
    push DS                                   ; 1e
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    call 0e03fh                               ; e8 b6 f0
    or byte [0043eh], 080h                    ; 80 0e 3e 04 80
    pop DS                                    ; 1f
    pop dx                                    ; 5a
    pop ax                                    ; 58
    iret                                      ; cf
    times 0x33 db 0
    db  'XM'
_diskette_param_table:                       ; 0xfefc7 LB 0xd
    scasw                                     ; af
    add ah, byte [di]                         ; 02 25
    add dl, byte [bp+si]                      ; 02 12
    db  01bh, 0ffh
    ; sbb di, di                                ; 1b ff
    insb                                      ; 6c
    db  0f6h
    invd                                      ; 0f 08
    jmp short 0efd4h                          ; eb 00
int17_handler:                               ; 0xfefd4 LB 0xd
    push DS                                   ; 1e
    push ES                                   ; 06
    pushaw                                    ; 60
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 06e0bh                               ; e8 2e 7e
    popaw                                     ; 61
    pop ES                                    ; 07
    pop DS                                    ; 1f
    iret                                      ; cf
_pmode_IDT:                                  ; 0xfefe1 LB 0x6
    db  000h, 000h, 000h, 000h, 00fh, 000h
_rmode_IDT:                                  ; 0xfefe7 LB 0x6
    db  0ffh, 003h, 000h, 000h, 000h, 000h
int1c_handler:                               ; 0xfefed LB 0x78
    iret                                      ; cf
    times 0x55 db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    iret                                      ; cf
    times 0x1d db 0
    db  'XM'
int10_handler:                               ; 0xff065 LB 0x47
    iret                                      ; cf
    times 0x3c db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 01700h                               ; e8 56 26
    hlt                                       ; f4
    iret                                      ; cf
int19_relocated:                             ; 0xff0ac LB 0x90
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    mov ax, word [bp+002h]                    ; 8b 46 02
    cmp ax, 0f000h                            ; 3d 00 f0
    je short 0f0c3h                           ; 74 0d
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov ax, 01234h                            ; b8 34 12
    mov word [001d8h], ax                     ; a3 d8 01
    jmp near 0e05bh                           ; e9 98 ef
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    push bp                                   ; 55
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    mov ax, strict word 00001h                ; b8 01 00
    push ax                                   ; 50
    call 046d0h                               ; e8 00 56
    inc sp                                    ; 44
    inc sp                                    ; 44
    test ax, ax                               ; 85 c0
    jne short 0f0feh                          ; 75 28
    mov ax, strict word 00002h                ; b8 02 00
    push ax                                   ; 50
    call 046d0h                               ; e8 f3 55
    inc sp                                    ; 44
    inc sp                                    ; 44
    test ax, ax                               ; 85 c0
    jne short 0f0feh                          ; 75 1b
    mov ax, strict word 00003h                ; b8 03 00
    push strict byte 00003h                   ; 6a 03
    call 046d0h                               ; e8 e5 55
    inc sp                                    ; 44
    inc sp                                    ; 44
    test ax, ax                               ; 85 c0
    jne short 0f0feh                          ; 75 0d
    mov ax, strict word 00004h                ; b8 04 00
    push ax                                   ; 50
    call 046d0h                               ; e8 d8 55
    inc sp                                    ; 44
    inc sp                                    ; 44
    test ax, ax                               ; 85 c0
    je short 0f0a4h                           ; 74 a6
    sal eax, 004h                             ; 66 c1 e0 04
    mov word [bp+002h], ax                    ; 89 46 02
    shr eax, 004h                             ; 66 c1 e8 04
    and ax, 0f000h                            ; 25 00 f0
    mov word [bp+004h], ax                    ; 89 46 04
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov es, ax                                ; 8e c0
    mov word [byte bp+000h], ax               ; 89 46 00
    mov ax, 0aa55h                            ; b8 55 aa
    pop bp                                    ; 5d
    iret                                      ; cf
    or cx, word [bp+si]                       ; 0b 0a
    or word [di], ax                          ; 09 05
    push eax                                  ; 66 50
    mov eax, strict dword 000800000h          ; 66 b8 00 00 80 00
    db  08bh, 0c3h
    ; mov ax, bx                                ; 8b c3
    sal eax, 008h                             ; 66 c1 e0 08
    and dl, 0fch                              ; 80 e2 fc
    db  00ah, 0c2h
    ; or al, dl                                 ; 0a c2
    mov dx, 00cf8h                            ; ba f8 0c
    out DX, eax                               ; 66 ef
    pop eax                                   ; 66 58
    retn                                      ; c3
pcibios_init_iomem_bases:                    ; 0xff13c LB 0x16
    push bp                                   ; 55
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    mov eax, strict dword 0e0000000h          ; 66 b8 00 00 00 e0
    push eax                                  ; 66 50
    mov ax, 0d000h                            ; b8 00 d0
    push ax                                   ; 50
    mov ax, strict word 00010h                ; b8 10 00
    push ax                                   ; 50
    mov bx, strict word 00008h                ; bb 08 00
pci_init_io_loop1:                           ; 0xff152 LB 0xe
    mov DL, strict byte 000h                  ; b2 00
    call 0f121h                               ; e8 ca ff
    mov dx, 00cfch                            ; ba fc 0c
    in ax, DX                                 ; ed
    cmp ax, strict byte 0ffffh                ; 83 f8 ff
    je short 0f199h                           ; 74 39
enable_iomem_space:                          ; 0xff160 LB 0x39
    mov DL, strict byte 004h                  ; b2 04
    call 0f121h                               ; e8 bc ff
    mov dx, 00cfch                            ; ba fc 0c
    in AL, DX                                 ; ec
    or AL, strict byte 007h                   ; 0c 07
    out DX, AL                                ; ee
    mov DL, strict byte 000h                  ; b2 00
    call 0f121h                               ; e8 b0 ff
    mov dx, 00cfch                            ; ba fc 0c
    in eax, DX                                ; 66 ed
    cmp eax, strict dword 020001022h          ; 66 3d 22 10 00 20
    jne short 0f199h                          ; 75 1b
    mov DL, strict byte 010h                  ; b2 10
    call 0f121h                               ; e8 9e ff
    mov dx, 00cfch                            ; ba fc 0c
    in ax, DX                                 ; ed
    and ax, strict byte 0fffch                ; 83 e0 fc
    db  08bh, 0c8h
    ; mov cx, ax                                ; 8b c8
    db  08bh, 0d1h
    ; mov dx, cx                                ; 8b d1
    add dx, strict byte 00014h                ; 83 c2 14
    in ax, DX                                 ; ed
    db  08bh, 0d1h
    ; mov dx, cx                                ; 8b d1
    add dx, strict byte 00018h                ; 83 c2 18
    in eax, DX                                ; 66 ed
next_pci_dev:                                ; 0xff199 LB 0xf
    mov byte [bp-008h], 010h                  ; c6 46 f8 10
    inc bx                                    ; 43
    cmp bx, 00100h                            ; 81 fb 00 01
    jne short 0f152h                          ; 75 ae
    db  08bh, 0e5h
    ; mov sp, bp                                ; 8b e5
    pop bp                                    ; 5d
    retn                                      ; c3
pcibios_init_set_elcr:                       ; 0xff1a8 LB 0xc
    push ax                                   ; 50
    push cx                                   ; 51
    mov dx, 004d0h                            ; ba d0 04
    test AL, strict byte 008h                 ; a8 08
    je short 0f1b4h                           ; 74 03
    inc dx                                    ; 42
    and AL, strict byte 007h                  ; 24 07
is_master_pic:                               ; 0xff1b4 LB 0xd
    db  08ah, 0c8h
    ; mov cl, al                                ; 8a c8
    mov BL, strict byte 001h                  ; b3 01
    sal bl, CL                                ; d2 e3
    in AL, DX                                 ; ec
    db  00ah, 0c3h
    ; or al, bl                                 ; 0a c3
    out DX, AL                                ; ee
    pop cx                                    ; 59
    pop ax                                    ; 58
    retn                                      ; c3
pcibios_init_irqs:                           ; 0xff1c1 LB 0x53
    push DS                                   ; 1e
    push bp                                   ; 55
    mov ax, 0f000h                            ; b8 00 f0
    mov ds, ax                                ; 8e d8
    mov dx, 004d0h                            ; ba d0 04
    mov AL, strict byte 000h                  ; b0 00
    out DX, AL                                ; ee
    inc dx                                    ; 42
    out DX, AL                                ; ee
    mov si, 0f2a0h                            ; be a0 f2
    mov bh, byte [si+008h]                    ; 8a 7c 08
    mov bl, byte [si+009h]                    ; 8a 5c 09
    mov DL, strict byte 000h                  ; b2 00
    call 0f121h                               ; e8 43 ff
    mov dx, 00cfch                            ; ba fc 0c
    in eax, DX                                ; 66 ed
    cmp eax, dword [si+00ch]                  ; 66 3b 44 0c
    jne near 0f291h                           ; 0f 85 a6 00
    mov dl, byte [si+022h]                    ; 8a 54 22
    call 0f121h                               ; e8 30 ff
    push bx                                   ; 53
    mov dx, 00cfch                            ; ba fc 0c
    mov ax, 08080h                            ; b8 80 80
    out DX, ax                                ; ef
    add dx, strict byte 00002h                ; 83 c2 02
    out DX, ax                                ; ef
    mov ax, word [si+006h]                    ; 8b 44 06
    sub ax, strict byte 00020h                ; 83 e8 20
    shr ax, 004h                              ; c1 e8 04
    db  08bh, 0c8h
    ; mov cx, ax                                ; 8b c8
    add si, strict byte 00020h                ; 83 c6 20
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    mov ax, 0f11dh                            ; b8 1d f1
    push ax                                   ; 50
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    push ax                                   ; 50
pci_init_irq_loop1:                          ; 0xff214 LB 0x5
    mov bh, byte [si]                         ; 8a 3c
    mov bl, byte [si+001h]                    ; 8a 5c 01
pci_init_irq_loop2:                          ; 0xff219 LB 0x15
    mov DL, strict byte 000h                  ; b2 00
    call 0f121h                               ; e8 03 ff
    mov dx, 00cfch                            ; ba fc 0c
    in ax, DX                                 ; ed
    cmp ax, strict byte 0ffffh                ; 83 f8 ff
    jne short 0f22eh                          ; 75 07
    test bl, 007h                             ; f6 c3 07
    je short 0f285h                           ; 74 59
    jmp short 0f27bh                          ; eb 4d
pci_test_int_pin:                            ; 0xff22e LB 0x3c
    mov DL, strict byte 03ch                  ; b2 3c
    call 0f121h                               ; e8 ee fe
    mov dx, 00cfdh                            ; ba fd 0c
    in AL, DX                                 ; ec
    and AL, strict byte 007h                  ; 24 07
    je short 0f27bh                           ; 74 40
    db  0feh, 0c8h
    ; dec al                                    ; fe c8
    mov DL, strict byte 003h                  ; b2 03
    mul dl                                    ; f6 e2
    add AL, strict byte 002h                  ; 04 02
    db  032h, 0e4h
    ; xor ah, ah                                ; 32 e4
    db  08bh, 0d8h
    ; mov bx, ax                                ; 8b d8
    mov al, byte [bx+si]                      ; 8a 00
    db  08ah, 0d0h
    ; mov dl, al                                ; 8a d0
    mov bx, word [byte bp+000h]               ; 8b 5e 00
    call 0f121h                               ; e8 d0 fe
    mov dx, 00cfch                            ; ba fc 0c
    and AL, strict byte 003h                  ; 24 03
    db  002h, 0d0h
    ; add dl, al                                ; 02 d0
    in AL, DX                                 ; ec
    cmp AL, strict byte 080h                  ; 3c 80
    jc short 0f26ah                           ; 72 0d
    mov bx, word [bp-002h]                    ; 8b 5e fe
    mov al, byte [bx]                         ; 8a 07
    out DX, AL                                ; ee
    inc bx                                    ; 43
    mov word [bp-002h], bx                    ; 89 5e fe
    call 0f1a8h                               ; e8 3e ff
pirq_found:                                  ; 0xff26a LB 0x11
    mov bh, byte [si]                         ; 8a 3c
    mov bl, byte [si+001h]                    ; 8a 5c 01
    add bl, byte [bp-003h]                    ; 02 5e fd
    mov DL, strict byte 03ch                  ; b2 3c
    call 0f121h                               ; e8 aa fe
    mov dx, 00cfch                            ; ba fc 0c
    out DX, AL                                ; ee
next_pci_func:                               ; 0xff27b LB 0xa
    inc byte [bp-003h]                        ; fe 46 fd
    db  0feh, 0c3h
    ; inc bl                                    ; fe c3
    test bl, 007h                             ; f6 c3 07
    jne short 0f219h                          ; 75 94
next_pir_entry:                              ; 0xff285 LB 0xc
    add si, strict byte 00010h                ; 83 c6 10
    mov byte [bp-003h], 000h                  ; c6 46 fd 00
    loop 0f214h                               ; e2 86
    db  08bh, 0e5h
    ; mov sp, bp                                ; 8b e5
    pop bx                                    ; 5b
pci_init_end:                                ; 0xff291 LB 0x2f
    pop bp                                    ; 5d
    pop DS                                    ; 1f
    retn                                      ; c3
    db  089h, 0c0h, 089h, 0c0h, 089h, 0c0h, 089h, 0c0h, 089h, 0c0h, 089h, 0c0h, 024h, 050h, 049h, 052h
    db  000h, 001h, 000h, 002h, 000h, 008h, 000h, 000h, 086h, 080h, 000h, 070h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 031h
_pci_routing_table:                          ; 0xff2c0 LB 0x581
    db  000h, 008h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 000h, 000h
    db  000h, 010h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 001h, 000h
    db  000h, 018h, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 002h, 000h
    db  000h, 020h, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 003h, 000h
    db  000h, 028h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 004h, 000h
    db  000h, 030h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 005h, 000h
    db  000h, 038h, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 006h, 000h
    db  000h, 040h, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 007h, 000h
    db  000h, 048h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 008h, 000h
    db  000h, 050h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 009h, 000h
    db  000h, 058h, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 00ah, 000h
    db  000h, 060h, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 00bh, 000h
    db  000h, 068h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 00ch, 000h
    db  000h, 070h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 00dh, 000h
    db  000h, 078h, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 00eh, 000h
    db  000h, 080h, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 00fh, 000h
    db  000h, 088h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 010h, 000h
    db  000h, 090h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 011h, 000h
    db  000h, 098h, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 012h, 000h
    db  000h, 0a0h, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 013h, 000h
    db  000h, 0a8h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 014h, 000h
    db  000h, 0b0h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 015h, 000h
    db  000h, 0b8h, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 016h, 000h
    db  000h, 0c0h, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 017h, 000h
    db  000h, 0c8h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 018h, 000h
    db  000h, 0d0h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 019h, 000h
    db  000h, 0d8h, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 01ah, 000h
    db  000h, 0e0h, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 01bh, 000h
    db  000h, 0e8h, 060h, 0f8h, 0deh, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 01ch, 000h
    db  000h, 0f0h, 061h, 0f8h, 0deh, 062h, 0f8h, 0deh, 063h, 0f8h, 0deh, 060h, 0f8h, 0deh, 01dh, 000h
    db  0e0h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 058h
    db  04dh
int12_handler:                               ; 0xff841 LB 0xc
    sti                                       ; fb
    push DS                                   ; 1e
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov ax, word [00013h]                     ; a1 13 00
    pop DS                                    ; 1f
    iret                                      ; cf
int11_handler:                               ; 0xff84d LB 0xc
    sti                                       ; fb
    push DS                                   ; 1e
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov ax, word [00010h]                     ; a1 10 00
    pop DS                                    ; 1f
    iret                                      ; cf
int15_handler:                               ; 0xff859 LB 0x29
    pushfw                                    ; 9c
    push DS                                   ; 1e
    push ES                                   ; 06
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    cmp ah, 086h                              ; 80 fc 86
    je short 0f887h                           ; 74 23
    cmp ah, 0e8h                              ; 80 fc e8
    je short 0f887h                           ; 74 1e
    pushaw                                    ; 60
    cmp ah, 053h                              ; 80 fc 53
    je short 0f87dh                           ; 74 0e
    cmp ah, 0c2h                              ; 80 fc c2
    je short 0f882h                           ; 74 0e
    call 05e6ch                               ; e8 f5 65
    popaw                                     ; 61
    pop ES                                    ; 07
    pop DS                                    ; 1f
    popfw                                     ; 9d
    jmp short 0f890h                          ; eb 13
    call 0887dh                               ; e8 fd 8f
    jmp short 0f877h                          ; eb f5
int15_handler_mouse:                         ; 0xff882 LB 0x5
    call 06a80h                               ; e8 fb 71
    jmp short 0f877h                          ; eb f0
int15_handler32:                             ; 0xff887 LB 0x9
    pushad                                    ; 66 60
    call 06339h                               ; e8 ad 6a
    popad                                     ; 66 61
    jmp short 0f878h                          ; eb e8
iret_modify_cf:                              ; 0xff890 LB 0x14
    jc short 0f89bh                           ; 72 09
    push bp                                   ; 55
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    and byte [bp+006h], 0feh                  ; 80 66 06 fe
    pop bp                                    ; 5d
    iret                                      ; cf
    push bp                                   ; 55
    db  08bh, 0ech
    ; mov bp, sp                                ; 8b ec
    or byte [bp+006h], 001h                   ; 80 4e 06 01
    pop bp                                    ; 5d
    iret                                      ; cf
int74_handler:                               ; 0xff8a4 LB 0x2e
    sti                                       ; fb
    pushaw                                    ; 60
    push ES                                   ; 06
    push DS                                   ; 1e
    push strict byte 00000h                   ; 6a 00
    push strict byte 00000h                   ; 6a 00
    push strict byte 00000h                   ; 6a 00
    push strict byte 00000h                   ; 6a 00
    push strict byte 00000h                   ; 6a 00
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 069b6h                               ; e8 fe 70
    pop cx                                    ; 59
    jcxz 0f8c7h                               ; e3 0c
    push strict byte 00000h                   ; 6a 00
    pop DS                                    ; 1f
    push word [0040eh]                        ; ff 36 0e 04
    pop DS                                    ; 1f
    call far [word 00022h]                    ; ff 1e 22 00
    cli                                       ; fa
    call 0e03bh                               ; e8 70 e7
    add sp, strict byte 00008h                ; 83 c4 08
    pop DS                                    ; 1f
    pop ES                                    ; 07
    popaw                                     ; 61
    iret                                      ; cf
int76_handler:                               ; 0xff8d2 LB 0x19c
    push ax                                   ; 50
    push DS                                   ; 1e
    mov ax, strict word 00040h                ; b8 40 00
    mov ds, ax                                ; 8e d8
    mov byte [0008eh], 0ffh                   ; c6 06 8e 00 ff
    call 0e03bh                               ; e8 5a e7
    pop DS                                    ; 1f
    pop ax                                    ; 58
    iret                                      ; cf
    times 0x188 db 0
    db  'XM'
font8x8:                                     ; 0xffa6e LB 0x421
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 07eh, 081h, 0a5h, 081h, 0bdh, 099h, 081h, 07eh
    db  07eh, 0ffh, 0dbh, 0ffh, 0c3h, 0e7h, 0ffh, 07eh, 06ch, 0feh, 0feh, 0feh, 07ch, 038h, 010h, 000h
    db  010h, 038h, 07ch, 0feh, 07ch, 038h, 010h, 000h, 038h, 07ch, 038h, 0feh, 0feh, 07ch, 038h, 07ch
    db  010h, 010h, 038h, 07ch, 0feh, 07ch, 038h, 07ch, 000h, 000h, 018h, 03ch, 03ch, 018h, 000h, 000h
    db  0ffh, 0ffh, 0e7h, 0c3h, 0c3h, 0e7h, 0ffh, 0ffh, 000h, 03ch, 066h, 042h, 042h, 066h, 03ch, 000h
    db  0ffh, 0c3h, 099h, 0bdh, 0bdh, 099h, 0c3h, 0ffh, 00fh, 007h, 00fh, 07dh, 0cch, 0cch, 0cch, 078h
    db  03ch, 066h, 066h, 066h, 03ch, 018h, 07eh, 018h, 03fh, 033h, 03fh, 030h, 030h, 070h, 0f0h, 0e0h
    db  07fh, 063h, 07fh, 063h, 063h, 067h, 0e6h, 0c0h, 099h, 05ah, 03ch, 0e7h, 0e7h, 03ch, 05ah, 099h
    db  080h, 0e0h, 0f8h, 0feh, 0f8h, 0e0h, 080h, 000h, 002h, 00eh, 03eh, 0feh, 03eh, 00eh, 002h, 000h
    db  018h, 03ch, 07eh, 018h, 018h, 07eh, 03ch, 018h, 066h, 066h, 066h, 066h, 066h, 000h, 066h, 000h
    db  07fh, 0dbh, 0dbh, 07bh, 01bh, 01bh, 01bh, 000h, 03eh, 063h, 038h, 06ch, 06ch, 038h, 0cch, 078h
    db  000h, 000h, 000h, 000h, 07eh, 07eh, 07eh, 000h, 018h, 03ch, 07eh, 018h, 07eh, 03ch, 018h, 0ffh
    db  018h, 03ch, 07eh, 018h, 018h, 018h, 018h, 000h, 018h, 018h, 018h, 018h, 07eh, 03ch, 018h, 000h
    db  000h, 018h, 00ch, 0feh, 00ch, 018h, 000h, 000h, 000h, 030h, 060h, 0feh, 060h, 030h, 000h, 000h
    db  000h, 000h, 0c0h, 0c0h, 0c0h, 0feh, 000h, 000h, 000h, 024h, 066h, 0ffh, 066h, 024h, 000h, 000h
    db  000h, 018h, 03ch, 07eh, 0ffh, 0ffh, 000h, 000h, 000h, 0ffh, 0ffh, 07eh, 03ch, 018h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 030h, 078h, 078h, 030h, 030h, 000h, 030h, 000h
    db  06ch, 06ch, 06ch, 000h, 000h, 000h, 000h, 000h, 06ch, 06ch, 0feh, 06ch, 0feh, 06ch, 06ch, 000h
    db  030h, 07ch, 0c0h, 078h, 00ch, 0f8h, 030h, 000h, 000h, 0c6h, 0cch, 018h, 030h, 066h, 0c6h, 000h
    db  038h, 06ch, 038h, 076h, 0dch, 0cch, 076h, 000h, 060h, 060h, 0c0h, 000h, 000h, 000h, 000h, 000h
    db  018h, 030h, 060h, 060h, 060h, 030h, 018h, 000h, 060h, 030h, 018h, 018h, 018h, 030h, 060h, 000h
    db  000h, 066h, 03ch, 0ffh, 03ch, 066h, 000h, 000h, 000h, 030h, 030h, 0fch, 030h, 030h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 030h, 030h, 060h, 000h, 000h, 000h, 0fch, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 030h, 030h, 000h, 006h, 00ch, 018h, 030h, 060h, 0c0h, 080h, 000h
    db  07ch, 0c6h, 0ceh, 0deh, 0f6h, 0e6h, 07ch, 000h, 030h, 070h, 030h, 030h, 030h, 030h, 0fch, 000h
    db  078h, 0cch, 00ch, 038h, 060h, 0cch, 0fch, 000h, 078h, 0cch, 00ch, 038h, 00ch, 0cch, 078h, 000h
    db  01ch, 03ch, 06ch, 0cch, 0feh, 00ch, 01eh, 000h, 0fch, 0c0h, 0f8h, 00ch, 00ch, 0cch, 078h, 000h
    db  038h, 060h, 0c0h, 0f8h, 0cch, 0cch, 078h, 000h, 0fch, 0cch, 00ch, 018h, 030h, 030h, 030h, 000h
    db  078h, 0cch, 0cch, 078h, 0cch, 0cch, 078h, 000h, 078h, 0cch, 0cch, 07ch, 00ch, 018h, 070h, 000h
    db  000h, 030h, 030h, 000h, 000h, 030h, 030h, 000h, 000h, 030h, 030h, 000h, 000h, 030h, 030h, 060h
    db  018h, 030h, 060h, 0c0h, 060h, 030h, 018h, 000h, 000h, 000h, 0fch, 000h, 000h, 0fch, 000h, 000h
    db  060h, 030h, 018h, 00ch, 018h, 030h, 060h, 000h, 078h, 0cch, 00ch, 018h, 030h, 000h, 030h, 000h
    db  07ch, 0c6h, 0deh, 0deh, 0deh, 0c0h, 078h, 000h, 030h, 078h, 0cch, 0cch, 0fch, 0cch, 0cch, 000h
    db  0fch, 066h, 066h, 07ch, 066h, 066h, 0fch, 000h, 03ch, 066h, 0c0h, 0c0h, 0c0h, 066h, 03ch, 000h
    db  0f8h, 06ch, 066h, 066h, 066h, 06ch, 0f8h, 000h, 0feh, 062h, 068h, 078h, 068h, 062h, 0feh, 000h
    db  0feh, 062h, 068h, 078h, 068h, 060h, 0f0h, 000h, 03ch, 066h, 0c0h, 0c0h, 0ceh, 066h, 03eh, 000h
    db  0cch, 0cch, 0cch, 0fch, 0cch, 0cch, 0cch, 000h, 078h, 030h, 030h, 030h, 030h, 030h, 078h, 000h
    db  01eh, 00ch, 00ch, 00ch, 0cch, 0cch, 078h, 000h, 0e6h, 066h, 06ch, 078h, 06ch, 066h, 0e6h, 000h
    db  0f0h, 060h, 060h, 060h, 062h, 066h, 0feh, 000h, 0c6h, 0eeh, 0feh, 0feh, 0d6h, 0c6h, 0c6h, 000h
    db  0c6h, 0e6h, 0f6h, 0deh, 0ceh, 0c6h, 0c6h, 000h, 038h, 06ch, 0c6h, 0c6h, 0c6h, 06ch, 038h, 000h
    db  0fch, 066h, 066h, 07ch, 060h, 060h, 0f0h, 000h, 078h, 0cch, 0cch, 0cch, 0dch, 078h, 01ch, 000h
    db  0fch, 066h, 066h, 07ch, 06ch, 066h, 0e6h, 000h, 078h, 0cch, 0e0h, 070h, 01ch, 0cch, 078h, 000h
    db  0fch, 0b4h, 030h, 030h, 030h, 030h, 078h, 000h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 0fch, 000h
    db  0cch, 0cch, 0cch, 0cch, 0cch, 078h, 030h, 000h, 0c6h, 0c6h, 0c6h, 0d6h, 0feh, 0eeh, 0c6h, 000h
    db  0c6h, 0c6h, 06ch, 038h, 038h, 06ch, 0c6h, 000h, 0cch, 0cch, 0cch, 078h, 030h, 030h, 078h, 000h
    db  0feh, 0c6h, 08ch, 018h, 032h, 066h, 0feh, 000h, 078h, 060h, 060h, 060h, 060h, 060h, 078h, 000h
    db  0c0h, 060h, 030h, 018h, 00ch, 006h, 002h, 000h, 078h, 018h, 018h, 018h, 018h, 018h, 078h, 000h
    db  010h, 038h, 06ch, 0c6h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh
    db  030h, 030h, 018h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 078h, 00ch, 07ch, 0cch, 076h, 000h
    db  0e0h, 060h, 060h, 07ch, 066h, 066h, 0dch, 000h, 000h, 000h, 078h, 0cch, 0c0h, 0cch, 078h, 000h
    db  01ch, 00ch, 00ch, 07ch, 0cch, 0cch, 076h, 000h, 000h, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h
    db  038h, 06ch, 060h, 0f0h, 060h, 060h, 0f0h, 000h, 000h, 000h, 076h, 0cch, 0cch, 07ch, 00ch, 0f8h
    db  0e0h, 060h, 06ch, 076h, 066h, 066h, 0e6h, 000h, 030h, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    db  00ch, 000h, 00ch, 00ch, 00ch, 0cch, 0cch, 078h, 0e0h, 060h, 066h, 06ch, 078h, 06ch, 0e6h, 000h
    db  070h, 030h, 030h, 030h, 030h, 030h, 078h, 000h, 000h, 000h, 0cch, 0feh, 0feh, 0d6h, 0c6h, 000h
    db  000h, 000h, 0f8h, 0cch, 0cch, 0cch, 0cch, 000h, 000h, 000h, 078h, 0cch, 0cch, 0cch, 078h, 000h
    db  000h, 000h, 0dch, 066h, 066h, 07ch, 060h, 0f0h, 000h, 000h, 076h, 0cch, 0cch, 07ch, 00ch, 01eh
    db  000h, 000h, 0dch, 076h, 066h, 060h, 0f0h, 000h, 000h, 000h, 07ch, 0c0h, 078h, 00ch, 0f8h, 000h
    db  010h, 030h, 07ch, 030h, 030h, 034h, 018h, 000h, 000h, 000h, 0cch, 0cch, 0cch, 0cch, 076h, 000h
    db  000h, 000h, 0cch, 0cch, 0cch, 078h, 030h, 000h, 000h, 000h, 0c6h, 0d6h, 0feh, 0feh, 06ch, 000h
    db  000h, 000h, 0c6h, 06ch, 038h, 06ch, 0c6h, 000h, 000h, 000h, 0cch, 0cch, 0cch, 07ch, 00ch, 0f8h
    db  000h, 000h, 0fch, 098h, 030h, 064h, 0fch, 000h, 01ch, 030h, 030h, 0e0h, 030h, 030h, 01ch, 000h
    db  018h, 018h, 018h, 000h, 018h, 018h, 018h, 000h, 0e0h, 030h, 030h, 01ch, 030h, 030h, 0e0h, 000h
    db  076h, 0dch, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 000h
    db  080h, 0fch, 0b1h, 075h, 00fh, 006h, 01eh, 00eh, 01fh, 0fch, 066h, 060h, 0e8h, 010h, 08ch, 066h
    db  061h, 01fh, 007h, 0cfh, 006h, 01eh, 060h, 00eh, 01fh, 0fch, 0e8h, 0c2h, 068h, 061h, 01fh, 007h
    db  0cfh
int70_handler:                               ; 0xffe8f LB 0x16
    push ES                                   ; 06
    push DS                                   ; 1e
    pushaw                                    ; 60
    push CS                                   ; 0e
    pop DS                                    ; 1f
    cld                                       ; fc
    call 0668fh                               ; e8 f7 67
    popaw                                     ; 61
    pop DS                                    ; 1f
    pop ES                                    ; 07
    iret                                      ; cf
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si+04dh], bl                 ; 00 58 4d
int08_handler:                               ; 0xffea5 LB 0xae
    sti                                       ; fb
    push eax                                  ; 66 50
    push DS                                   ; 1e
    db  033h, 0c0h
    ; xor ax, ax                                ; 33 c0
    mov ds, ax                                ; 8e d8
    mov AL, byte [00440h]                     ; a0 40 04
    db  00ah, 0c0h
    ; or al, al                                 ; 0a c0
    je short 0febdh                           ; 74 09
    push dx                                   ; 52
    mov dx, 003f2h                            ; ba f2 03
    in AL, DX                                 ; ec
    and AL, strict byte 0cfh                  ; 24 cf
    out DX, AL                                ; ee
    pop dx                                    ; 5a
    mov eax, dword [0046ch]                   ; 66 a1 6c 04
    inc eax                                   ; 66 40
    cmp eax, strict dword 0001800b0h          ; 66 3d b0 00 18 00
    jc short 0fed2h                           ; 72 07
    db  066h, 033h, 0c0h
    ; xor eax, eax                              ; 66 33 c0
    inc byte [00470h]                         ; fe 06 70 04
    mov dword [0046ch], eax                   ; 66 a3 6c 04
    int 01ch                                  ; cd 1c
    cli                                       ; fa
    call 0e03fh                               ; e8 63 e1
    pop DS                                    ; 1f
    pop eax                                   ; 66 58
    iret                                      ; cf
    times 0x11 db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    times 0xb db 0
    pop ax                                    ; 58
    dec bp                                    ; 4d
    dec di                                    ; 4f
    jc short 0ff64h                           ; 72 61
    arpl word [si+065h], bp                   ; 63 6c 65
    and byte [bp+04dh], dl                    ; 20 56 4d
    and byte [bp+069h], dl                    ; 20 56 69
    jc short 0ff82h                           ; 72 74
    jne short 0ff71h                          ; 75 61
    insb                                      ; 6c
    inc dx                                    ; 42
    outsw                                     ; 6f
    js short 0ff35h                           ; 78 20
    inc dx                                    ; 42
    dec cx                                    ; 49
    dec di                                    ; 4f
    push bx                                   ; 53
    times 0x38 db 0
    db  'XM'
dummy_iret:                                  ; 0xfff53 LB 0x9d
    iret                                      ; cf
    iret                                      ; cf
    mov ax, ax                                ; 89 c0
    mov ax, ax                                ; 89 c0
    mov ax, ax                                ; 89 c0
    mov ax, ax                                ; 89 c0
    mov ax, ax                                ; 89 c0
    cld                                       ; fc
    pop di                                    ; 5f
    push bx                                   ; 53
    dec bp                                    ; 4d
    pop di                                    ; 5f
    jnl short 0ff85h                          ; 7d 1f
    add al, byte [di]                         ; 02 05
    inc word [bx+si]                          ; ff 00
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si], al                      ; 00 00
    add byte [bx+si], al                      ; 00 00
    pop di                                    ; 5f
    inc sp                                    ; 44
    dec bp                                    ; 4d
    dec cx                                    ; 49
    pop di                                    ; 5f
    sbb AL, strict byte 000h                  ; 1c 00
    add byte [bx+si], al                      ; 00 00
    adc byte [00900h], cl                     ; 10 0e 00 09
    add byte [di], ah                         ; 00 25
    times 0x6f db 0
    db  'XM'
cpu_reset:                                   ; 0xffff0 LB 0x10
    jmp far 0f000h:0e05bh                     ; ea 5b e0 00 f0
    db  030h, 036h, 02fh, 032h, 033h, 02fh, 039h, 039h, 000h, 0fch, 0e7h
