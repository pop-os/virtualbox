#  Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.<BR>
#  Copyright (C) 2020, Red Hat, Inc.<BR>
#  Copyright (c) 2020, ARM Ltd. All rights reserved.<BR>
import email.header

class EmailAddressCheck:
    """Checks an email address."""

    def __init__(self, email, description):
        self.ok = True

        if email is None:
            self.error('Email address is missing!')
            return
        if description is None:
            self.error('Email description is missing!')
            return

        self.description = "'" + description + "'"
        self.check_email_address(email)

    def error(self, *err):
        if self.ok and Verbose.level > Verbose.ONELINE:
            print('The ' + self.description + ' email address is not valid:')
        self.ok = False
        if Verbose.level < Verbose.NORMAL:
            return
        count = 0
        for line in err:
            prefix = (' *', '  ')[count > 0]
            print(prefix, line)
            count += 1

    email_re1 = re.compile(r'(?:\s*)(.*?)(\s*)<(.+)>\s*$',
                           re.MULTILINE|re.IGNORECASE)

    def check_email_address(self, email):
        email = email.strip()
        mo = self.email_re1.match(email)
        if mo is None:
            self.error("Email format is invalid: " + email.strip())
            return

        name = mo.group(1).strip()
        if name == '':
            self.error("Name is not provided with email address: " +
                       email)
        else:
            quoted = len(name) > 2 and name[0] == '"' and name[-1] == '"'
            if name.find(',') >= 0 and not quoted:
                self.error('Add quotes (") around name with a comma: ' +
                           name)

        if mo.group(2) == '':
            self.error("There should be a space between the name and " +
                       "email address: " + email)

        if mo.group(3).find(' ') >= 0:
            self.error("The email address cannot contain a space: " +
                       mo.group(3))

        if ' via Groups.Io' in name and mo.group(3).endswith('@groups.io'):
            self.error("Email rewritten by lists DMARC / DKIM / SPF: " +
                       email)

        print (subject)

            EmailAddressCheck(s[3], sig)
    cve_re = re.compile('CVE-[0-9]{4}-[0-9]{5}[^0-9]')

        if count >= 1 and re.search(self.cve_re, lines[0]):
            #
            # If CVE-xxxx-xxxxx is present in subject line, then limit length of
            # subject line to 92 characters
            #
            if len(lines[0].rstrip()) >= 93:
                self.error(
                    'First line of commit message (subject line) is too long (%d >= 93).' %
                    (len(lines[0].rstrip()))
                    )
        else:
            #
            # If CVE-xxxx-xxxxx is not present in subject line, then limit
            # length of subject line to 75 characters
            #
            if len(lines[0].rstrip()) >= 76:
                self.error(
                    'First line of commit message (subject line) is too long (%d >= 76).' %
                    (len(lines[0].rstrip()))
                    )
                not lines[i].startswith('git-svn-id:') and
                not lines[i].startswith('Reviewed-by') and
                not lines[i].startswith('Acked-by:') and
                not lines[i].startswith('Tested-by:') and
                not lines[i].startswith('Reported-by:') and
                not lines[i].startswith('Suggested-by:') and
                not lines[i].startswith('Signed-off-by:') and
                not lines[i].startswith('Cc:')):
                #
                # Print a warning if body line is longer than 75 characters
                #
                print(
                    'WARNING - Line %d of commit message is too long (%d >= 76).' %
                    (i + 1, len(lines[i]))
                    )
                print(lines[i])
                self.force_crlf = True
                self.force_notabs = True
                if self.filename.endswith('.sh') or \
                    self.filename.startswith('BaseTools/BinWrappers/PosixLike/') or \
                    self.filename.startswith('BaseTools/BinPipWrappers/PosixLike/') or \
                    self.filename.startswith('BaseTools/Bin/CYGWIN_NT-5.1-i686/') or \
                    self.filename == 'BaseTools/BuildEnv':
                    #
                    # Do not enforce CR/LF line endings for linux shell scripts.
                    # Some linux shell scripts don't end with the ".sh" extension,
                    # they are identified by their path.
                    #
                    self.force_crlf = False
                if self.filename == '.gitmodules' or \
                   self.filename == 'BaseTools/Conf/diff.order':
                    #
                    # .gitmodules and diff orderfiles are used internally by git
                    # use tabs and LF line endings.  Do not enforce no tabs and
                    # do not enforce CR/LF line endings.
                    #
                    self.force_crlf = False
                    self.force_notabs = False
            elif line.startswith('new file mode 160000'):
                #
                # New submodule.  Do not enforce CR/LF line endings
                #
                self.force_crlf = False
        if self.force_crlf and eol != '\r\n' and (line.find('Subproject commit') == -1):
        if self.force_notabs and '\t' in line:
        email_check = EmailAddressCheck(self.author_email, 'Author')
        email_ok = email_check.ok

        self.ok = email_ok and msg_ok and diff_ok
        #
        # Parse subject line from email header.  The subject line may be
        # composed of multiple parts with different encodings.  Decode and
        # combine all the parts to produce a single string with the contents of
        # the decoded subject line.
        #
        parts = email.header.decode_header(pmail.get('subject'))
        subject = ''
        for (part, encoding) in parts:
            if encoding:
                part = part.decode(encoding)
            else:
                try:
                    part = part.decode()
                except:
                    pass
            subject = subject + part
        self.commit_subject = subject.replace('\r\n', '')
        self.author_email = pmail['from']

            email = self.read_committer_email_address_from_git(commit)
            self.ok &= EmailAddressCheck(email, 'Committer').ok
        return self.run_git('show', '--pretty=email', '--no-textconv',
                            '--no-use-mailmap', commit)

    def read_committer_email_address_from_git(self, commit):
        # Run git to get the committer email
        return self.run_git('show', '--pretty=%cn <%ce>', '--no-patch',
                            '--no-use-mailmap', commit)