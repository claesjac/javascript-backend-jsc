#!/usr/bin/perl

use strict;
use warnings;
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
is($cx->eval("0;"), 0);
is($cx->eval("-1.1;"), -1.1);
is($cx->eval("1.1;"), 1.1);

# Strings
my $str = $cx->eval("'with no funny chars';");
is($str, "with no funny chars");

# Anon objects
my $obj = $cx->eval("v = {a: 1, b: 2}; v", { ConvertObjects => 1 });
is_deeply($obj, {a => 1, b => 2});

$obj = $cx->eval("v = {a: 1, b: 2}; v", { ConvertObjects => 0 });
isa_ok($obj, "JavaScript::Backend::JSC::Object");
isa_ok($obj, "JavaScript::Object");

# Anon arrays
my $arr = $cx->eval("w = [1,2,3]; w", { ConvertArrays => 1 });
is_deeply($arr, [1,2,3]);

$arr = $cx->eval("w [1,2,3]; w", { ConvertArrays => 0 });
isa_ok($arr, "JavaScript::Backend::JSC::Array");
isa_ok($arr, "JavaScript::Array");
