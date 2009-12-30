#!/usr/bin/perl

use strict;
use warnings;

use Test::More qw(no_plan);

use JavaScript::Backend::JSC;

my $rt = JavaScript::Backend::JSC::Runtime->new();

ok(!$rt->has_context("MyContext"));

my $cx = $rt->create_context();
my $cx2 = $rt->create_named_context("foo");

is_deeply([$rt->get_context_names], [qw(ctx-0 foo)]);

ok(!$cx->get_option("ConvertObjects"));

$cx->set_option(ConvertObjects => 1);
ok($cx->get_option("ConvertObjects"));
