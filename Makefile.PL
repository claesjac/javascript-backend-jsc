use 5.006000;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.

if ($^O eq "darwin") {
    print "On OS X, link with framework\n";
}

WriteMakefile(
    NAME              => 'JavaScript::Backend::JSC',
    VERSION_FROM      => 'lib/JavaScript/Backend/JSC.pm', # finds $VERSION
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM  => 'lib/JavaScript/Backend/JSC.pm', # retrieve abstract from module
       AUTHOR         => 'Claes Jakobsson <claes@apple.com>') : ()),
    LIBS              => ['-framework JavaScriptCore'], # e.g., '-lm'
    DEFINE            => '', # e.g., '-DHAVE_SOMETHING'
    INC               => '-I. -FJavaScriptCore', # e.g., '-I. -I/usr/include/other
	# Un-comment this if you add C files to link with later:
    OBJECT            => '$(O_FILES)', # link all the C files too
    CCFLAGS           => '-g',
);
