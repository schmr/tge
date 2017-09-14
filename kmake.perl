#!/usr/bin/env perl
BEGIN{ $^W=1 }

$cfile=$ARGV[0];
system("cp $cfile ${cfile}_kmake");
system("cp ${cfile}_new.c $cfile");

system("gcc $ARGV[0] KSVtime.c");

system("cp ${cfile}_kmake ${cfile}");
