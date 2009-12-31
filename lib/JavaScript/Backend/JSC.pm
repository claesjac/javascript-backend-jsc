package JavaScript::Backend::JSC;

use 5.006000;
use strict;
use warnings;

require Exporter;
use AutoLoader qw(AUTOLOAD);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('JavaScript::Backend::JSC', $VERSION);

require JavaScript::Backend::JSC::Array;
require JavaScript::Backend::JSC::Context;
require JavaScript::Backend::JSC::Object;
require JavaScript::Backend::JSC::Runtime;

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

JavaScript::Backend::JSC - Perl extension for blah blah blah

=head1 SYNOPSIS

  use JavaScript::Backend::JSC;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for JavaScript::Backend::JSC, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Claes Jakobsson, E<lt>claes@apple.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2009 by Claes Jakobsson

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.10.0 or,
at your option, any later version of Perl 5 you may have available.


=cut
