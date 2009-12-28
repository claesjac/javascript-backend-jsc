#!/usr/bin/perl

use strict;
use warnings;

use Test::More qw(no_plan);

use JavaScript::Backend::JSC;

my $rt = JavaScript::Backend::JSC::Runtime->new();
isa_ok($rt, "JavaScript::Backend::JSC::Runtime");
isa_ok($rt, "JavaScript::Runtime");

my $rt2 = JavaScript::Backend::JSC::Runtime->new();
is($rt, $rt2);
