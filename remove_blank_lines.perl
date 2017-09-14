#!/usr/bin/env perl
BEGIN{ $^W=1 }

while (<>) {
  if (/\S+/) {
    print;
  }
}
