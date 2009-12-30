#!/usr/bin/perl

use strict;
use warnings;

use Test::More qw(no_plan);
use Test::Exception;

use JavaScript::Backend::JSC;

my $rt = JavaScript::Backend::JSC::Runtime->new();
my $cx = $rt->create_context();

lives_ok {
   $cx->eval("");
};

throws_ok {
    $cx->eval("", []);
} qr/options is not a hash reference/;

lives_ok {
    $cx->eval("", {});
};

ok(!$cx->get_option("ConvertObjects"));
$cx->eval("", { ConvertObjects => 1});
ok(!$cx->get_option("ConvertObjects"));
