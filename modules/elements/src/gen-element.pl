#@PERL_FOR_SCRIPTS@
#
# Copyright (C) 2007 Nicolas LAURENT
# This file is part of `Haplo'
# 
#
# `Haplo'  is free software;  you can  redistribute  it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation;  either version 2  of the License, or
# (at your option) any later version.
#
# `Haplo' is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
# for more details.
# 
# You should have  received  a copy of the   GNU General Public  License
# along with `Haplo'.  If not, write to  the
#
#                                        Free Software Foundation,  Inc.
#                                        675 Mass Ave, Cambridge, MA
#                                        02139, USA.
#


use strict;
use File::Basename;
my @FEATURES=(); # known feature (from element.h)
my %CARA; # known variables (from material.h and property.h)

my %FEATURE_FUNCTION;
my %FEATURE_INTEGR;
my %FEATURE_MATERIAL; # contains @tabular
my %FEATURE_PROPERTY; # contains @tabular
my %FEATURE_USER;
my %FEATURE_CODE;
my %USER_CODE; # local defined computation

my $NAME;
my $DESCR;
my $DOFSET;
my $NB_NODES = 0;
my $DIM = 0;
my $MODEL;


#####
# Remove double element in a list
#
# IN:
#    (REF) ARRAY
# OUT:
#####
sub uniq {
  my $ptr = shift;

  if ($ptr) {
    my @tab = @$ptr;
    my %hash;

    map { $hash{$_} = 1 } @tab;
    @$ptr = keys(%hash);
  }
}


sub push_uniq {
  my $ptr1=shift;
  my $ptr2=shift;

#DEBUG
#  print "push_uniq(", join(":", @$ptr1), ', ',join(":", @$ptr2), ")\n";
  foreach my $e (@$ptr2) {
    next if grep $e eq $_, @$ptr1;
    push(@$ptr1, $e);
  }
#DEBUG
#  print "-> ", join(":", @$ptr1), "\n";
}


#####
# Create a empty tabular and return a REF on it
#
# IN:
# OUT:
#    (REF) empty tabular
#####
sub empty_tab {
  my $ptr;
  my @tab=qw();
  $ptr = \@tab;

  return $ptr;
}


#####
#
# IN:
#     (string) directory that contents element.h file
#
# OUT:
#####
sub init {
  my $fem_include_dir = shift;

  my $element_h  = $fem_include_dir . '/element.h';
  my $material_h = $fem_include_dir . '/material.h';
  my $property_h = $fem_include_dir . '/property.h';
  # Initialize features

  open(IN, "< $element_h") || die "Cannot open $element_h: $!";
  while(<IN>) {
    chomp;
    if (m/^\s*FEM_ELEMENT_/) {
      s/^\s*FEM_ELEMENT_//;
      s/(=.*)?[,\s].*//;
      push(@FEATURES, $_);
    }
  }
  close(IN);
  pop(@FEATURES); # remove last entry


  open(IN, "< $material_h") || die "Cannot open $material_h: $!";
  while(<IN>) {
    chomp;
    if (m/^\s*FEM_MATERIAL_/) {
      s/^\s*FEM_MATERIAL_//;
      s/(=.*)?[,\s].*//;
      $CARA{$_} = 'MATERIAL';
    }
  }
  close(IN);

  open(IN, "< $property_h") || die "Cannot open $property_h: $!";
  while(<IN>) {
    chomp;
    if (m/^\s*FEM_PROPERTY_/) {
      s/^\s*FEM_PROPERTY_//;
      s/(=.*)?[,\s].*//;
      $CARA{$_} = 'PROPERTY';
    }
  }
  close(IN);


  foreach my $feature (@FEATURES) {
    $FEATURE_FUNCTION{$feature} = 'NULL';
    $FEATURE_INTEGR{$feature}   = 'NONE';
    $FEATURE_MATERIAL{$feature} = empty_tab();
    $FEATURE_PROPERTY{$feature} = empty_tab();
    $FEATURE_USER{$feature}     = empty_tab();
  }
}


#####
#
#####
sub input_translate {
  my $expression = shift;
  my $feature    = shift;

  my @tokens = split(/\b/, $expression);
  my $token;
  my $translated = '';

  foreach $token (@tokens) {

    my $transtoken;

    $transtoken=$token;

    # Trim key word
    $transtoken =~ s/^\s+//;
    $transtoken =~ s/\s+$//;

    # Now try to translate it
    # 1. CARA?
    my $cara = grep $_ eq "$token" , keys(%CARA);
    if ($cara) {
      my $tab;
      if ($CARA{$token} eq 'MATERIAL') {
	$tab = $FEATURE_MATERIAL{$feature};
	if (! $tab) {
	  $FEATURE_MATERIAL{$feature}=empty_tab();
	  $tab = $FEATURE_MATERIAL{$feature}
	}
      } elsif ($CARA{$token} eq 'PROPERTY') {
	$tab = $FEATURE_PROPERTY{$feature};
	if (! $tab) {
	  $FEATURE_PROPERTY{$feature} = empty_tab();
	  $tab = $FEATURE_PROPERTY{$feature};
	}	
      }
      my @tab_token = ($token);
      push_uniq($tab, \@tab_token);
    }

    # 2. USER defined sequence?
    my $user = grep $_ eq "$token" , keys(%USER_CODE);
    if ($user) {
      my $tab;
      $tab = $FEATURE_USER{$feature};
      if (!$tab) {
	$FEATURE_USER{$feature} = empty_tab();
	$tab = $FEATURE_USER{$feature};
      }
      my @tab_token = ($token);
      push_uniq($tab, \@tab_token);
    }


    $translated .= $transtoken;
  }

  return $translated;
}


#####
# parse input file
#
# IN:
#    (string) filename
# OUT:
#####
sub input {
  my $filename=shift;

  open(ELM, "< $filename");
  while(<ELM>) {
    chomp;

    # bypass comment
    s/#.*$//;

    # bypass empty lines
    next if (/^\s*$/);

    # Split KEY=VALUE
    my $indice=0;
    my ($key, $value) = split('=');

    # Trim key and value
    $key =~ s/^\s+//;
    $key =~ s/\s+$//;

    $value =~ s/^\s+//;
    $value =~ s/\s+$//;

    if ($key =~ m/(\w+)\s*\[(\d+)\]/) {
      $key = $1;
      $indice = $2;
    }

    # Switch on key
    if ($key eq 'NAME') {
      $NAME = $value;
    } elsif ($key eq 'DESCR') {
      $DESCR = $value;
    } elsif ($key eq 'DOFSET') {
      $DOFSET = $value;
    } elsif ($key eq 'NB_NODES') {
      $NB_NODES = $value;
    } elsif ($key eq 'DIM') {
      $DIM = $value;
    } elsif ($key eq 'MODEL') {
      $MODEL = $value;
    } elsif ($key =~ m/_INTEGR$/) {
      my $feature = $key;
      $feature =~ s/_.*$//;
      $FEATURE_INTEGR{$feature} = $value;
    } else {
      my $feature = grep $_ eq "$key", @FEATURES;
      if ($feature) {
	# This is a FEATURE
	$FEATURE_CODE{$key} .= "\tresult[$indice] = " . input_translate($value, $key) . ";\n";
      } else {
	# This is a LOCAL computation
	$USER_CODE{$key}    .= "\t$key = " . input_translate($value, $key) . ";\n";
      }
    }
  }
  close(ELM);
}

#####
# Print banner
#
# IN:
#     (scalar) filedescriptor
#     (string) banner title
# OUT:
#####
sub banner {
  my $unit = shift;
  my $name = uc(shift);

  $name =~ s/(.)/$1 /g;
  my $spaces = ' ' x (40 - length($name) / 2);
  print $unit '/*', '-' x 77, "\n";
  print $unit $spaces, $name, "\n";
  print $unit '-' x 77, "*/\n";
  print $unit "\n";
}


sub output_header {
  my $unit = shift;
  print $unit '/*', "\n";
  print $unit ' * Copyright (C) 2007 Nicolas LAURENT', "\n";
  print $unit ' * This file is part of `Haplo\'', "\n";
  print $unit ' * ', "\n";
  print $unit ' *', "\n";
  print $unit ' * `Haplo\'  is free software;  you can  redistribute  it and/or modify it', "\n";
  print $unit ' * under the terms of the GNU Library General Public License as published', "\n";
  print $unit ' * by the Free Software Foundation;  either version 2  of the License, or', "\n";
  print $unit ' * (at your option) any later version.', "\n";
  print $unit ' *', "\n";
  print $unit ' * `Haplo\' is distributed in the hope that it will be useful, but WITHOUT', "\n";
  print $unit ' * ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or', "\n";
  print $unit ' * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License', "\n";
  print $unit ' * for more details.', "\n";
  print $unit ' * ', "\n";
  print $unit ' * You should have  received  a copy of the   GNU General Public  License', "\n";
  print $unit ' * along with `Haplo\'.  If not, write to  the', "\n";
  print $unit ' *', "\n";
  print $unit ' *                                        Free Software Foundation,  Inc.', "\n";
  print $unit ' *                                        675 Mass Ave, Cambridge, MA', "\n";
  print $unit ' *                                        02139, USA.', "\n";
  print $unit ' *', "\n";
  print $unit ' */', "\n";
  print $unit "\n";
  print $unit '/*', "\n";
  print $unit ' * This code was generated by `genelem.pl\'', "\n";
  print $unit ' */', "\n";
  print $unit "\n";
}


#####
#
#####
sub output_include {
  my $unit = shift;

  print $unit '#include <math.h>', "\n";
  print $unit '#include <stdlib.h>', "\n";
  print $unit '#include <fem.h>', "\n\n";

  print $unit '#include "', $NAME, '.h"', "\n\n";
}


#####
#
#####
sub output_prototype {
  my $unit = shift;

  banner($unit, 'prototypes');

  foreach my $feature (@FEATURES) {
    if ($FEATURE_CODE{$feature}) {
      $FEATURE_FUNCTION{$feature} = $NAME . '_' . $feature;
      print $unit "static void $FEATURE_FUNCTION{$feature}(const fem_element_t *element, double *result);\n";
    }
  }
  print $unit "\n\n";
}


#####
#
#####
sub output_impl {
  my $unit = shift;

  banner($unit, 'implementation');

  foreach my $feature (@FEATURES) {
    if ($FEATURE_CODE{$feature}) {
      $FEATURE_FUNCTION{$feature} = $NAME . '_' . $feature;
      print "Feature: [$feature]\n";
      print $unit "/**\n";
      print $unit " * Elementary matrix for $feature\n";
      print $unit " *\n";
      print $unit " * \@param element is the considered element.\n";
      print $unit " * \@param result is a pointer to a correctly allocated buffer.\n";
      print $unit " */\n";
      print $unit "static void $FEATURE_FUNCTION{$feature}(const fem_element_t *element, double *result)\n";
      print $unit "{\n";

      my @feature_tab=();
      my $ptr;
      my @tab=($feature);


      do {
	@feature_tab = @tab;

	foreach my $f (@feature_tab) {
	  $ptr = $FEATURE_USER{$f};

	  #DEBUG
	  #print "\$FEATURE_USER{$f}=$ptr\n";
	  if ($ptr) {
	    push_uniq(\@tab, $ptr);
	  }
	}
      } while(@tab > @feature_tab);

      # MATERIAL
      @tab = ();
      foreach my $f (@feature_tab) {
	$ptr = $FEATURE_MATERIAL{$f};
	if ($ptr) {
	  push_uniq(\@tab, $ptr);
	}
      }

      my $ptr_mat = $FEATURE_MATERIAL{$feature};
      push_uniq($ptr_mat, \@tab);
      if (@tab) {
	foreach my $i (@tab) {
	  print $unit "\tFEM_MATERIAL_REQUEST(element, $i);\n";
	}
      }

      # PROPERTY
      @tab = ();
      foreach my $f (@feature_tab) {	
	$ptr = $FEATURE_PROPERTY{$f};
	if ($ptr) {
	  push_uniq(\@tab, \@$ptr);
	}
      }

      my $ptr_prop = $FEATURE_PROPERTY{$feature};
      push_uniq($ptr_prop, \@tab);
      if (@tab) {
	foreach my $i (@tab) {
	  print $unit "\tFEM_PROPERTY_REQUEST(element, $i);\n";
	}
      }

      # USER
      # Attention, feature is in this table.
      shift(@feature_tab);
      @feature_tab = reverse(@feature_tab);

      if (@feature_tab) {
	foreach my $i (@feature_tab) {
	  print $unit "\tdouble $i;\n";
	}
	print $unit "\n";
	foreach my $i (@feature_tab) {
	  print $unit $USER_CODE{$i};
	}
      }

      # COMPUTATION
      print $unit "\n";
      print $unit $FEATURE_CODE{$feature};
      print $unit "\n\treturn;\n";
      print $unit "}\n\n";
    }
  }
  print $unit "\n\n";
}


#####
# Write element descriptor
#
# IN:
#    (scalar) file descriptor
#####
sub output_footer {
  my $unit=shift;

  banner($unit, "global variables");
  print $unit "fem_element_type_t $NAME = {\n";
  print $unit "\t\"$NAME\", /* name */\n";
  print $unit "\t\"$DESCR\", /* description */\n";
  print $unit "\t\"$DOFSET\", /* dofset */\n";
  print $unit "\t$NB_NODES, /* number of nodes */\n";
  print $unit "\t$DIM, /* dimension of space */\n";
  print $unit "\tFEM_MODEL_$MODEL, /* type of model */\n";
  print $unit "\t{\n";
  foreach my $feature (@FEATURES) {
    print $unit "\t\t{ ";
    print $unit "$FEATURE_FUNCTION{$feature}, ";
    print $unit "FEM_INTEGRATION_$FEATURE_INTEGR{$feature}, ";

    my $tab;

    $tab = $FEATURE_MATERIAL{$feature};
    if ($tab) {
      print $unit '"', join(':', @$tab), '", ';
    } else {
      print $unit '"", ';
    }

    $tab = $FEATURE_PROPERTY{$feature};
    if ($tab) {
      print $unit '"', join(':', @$tab), '" ';
    } else {
      print $unit '"", ';
    }

    print $unit "}, /* $feature */ \n";
  }
  print $unit "\t}, /* features */\n";
  print $unit "\tNULL /* user value */\n";
  print $unit "};\n";
}


#####
#
#####
sub output_h {
  my $unit = shift;

  print $unit "#ifndef __ELEMENT_", $NAME, "_H__\n";
  print $unit "#define __ELEMENT_", $NAME, "_H__\n";
  print $unit "\n";
  print $unit '#include <fem/element.h>', "\n";
  print $unit "\n";
  print $unit "extern fem_element_type_t $NAME;\n";
  print $unit "\n";
  print $unit "#endif /* __ELEMENT_", $NAME, "_H__ */\n";
}


##
## USAGE
##
my $filename = $ARGV[0];
if (! $filename) {
  print "USAGE: $0 filename <fem module source root>\n";
  exit -1;
}
if (! ($filename =~ m/\.e$/)) {
  print $0, ": filename should have \`.e' suffixe.\n";
  exit -1;
}



##
## INIT
##
init($ARGV[1]);

##
## INPUT
##
input($filename);

##
## OUTPUT
##
my $c_file = basename($filename);
$c_file =~ s/\.e$/.c/;
print "--- GENERATING $NAME ---\n";
print 'C-file : "', $c_file, '"', "\n";
open(OUT, "> $c_file");
output_header(\*OUT);
output_include(\*OUT);
output_prototype(\*OUT);
output_impl(\*OUT);
output_footer(\*OUT);
close(OUT);

my $h_file = basename($filename);
$h_file =~ s/\.e$/.h/;
print 'H-file: "', $h_file, '"', "\n";
open(OUT, "> $h_file");
output_header(\*OUT);
output_h(\*OUT);
close(OUT);
print "--- END OF $NAME ---\n";

