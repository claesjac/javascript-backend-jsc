package JavaScript::Backend::JSC::Runtime;

use strict;
use warnings;

use base qw(JavaScript::Runtime);

sub get_context_names {
    my $self = shift;
    return keys %$self;
}

sub create_named_context {
    my ($self, $name) = @_;
    my $cx = $self->_create_named_context($name);
    $self->{$name} = $cx;
    $cx;
}
1;
__END__