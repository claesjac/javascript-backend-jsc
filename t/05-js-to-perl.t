#!/usr/bin/perl

use strict;
use warnings;
use utf8;
use Test::More qw(no_plan);

use JavaScript::Backend::JSC;

my $rt = JavaScript::Backend::JSC::Runtime->new();
my $cx = $rt->create_context();

# Undefined
ok(!defined $cx->eval("undefined;"));

# Booleans
ok($cx->eval("true;"));
ok(!$cx->eval("false;"));

# Numerics
is($cx->eval("1;"), 1);
is($cx->eval("-1;"), -1);
