#/******************************************************************************
# * SConstruct
# *
# * Source of KaHIP -- Karlsruhe High Quality Partitioning.
# *
# ******************************************************************************
# * Copyright (C) 2015 Christian Schulz <christian.schulz@kit.edu>
# *
# * This program is free software: you can redistribute it and/or modify it
# * under the terms of the GNU General Public License as published by the Free
# * Software Foundation, either version 5 of the License, or (at your option)
# * any later version.
# *
# * This program is distributed in the hope that it will be useful, but WITHOUT
# * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# * more details.
# *
# * You should have received a copy of the GNU General Public License along with
# * this program.  If not, see <http://www.gnu.org/licenses/>.
# *****************************************************************************/


# scons build file for the KaSVM.
#
# You can build it in the following variants:
#
#   optimized            no debug symbols, no assertions, optimization.
#   optimized_output     no debug symbols, no assertions, optimization -- more output on console.
#   debug                debug symbols, assertions, no optimization -- more output on console.
#
#   scons variant=${variant} program=${program}

import os
import platform
import sys

# Get the current platform.
SYSTEM = platform.uname()[0]
HOST = platform.uname()[1]

# Get shortcut to $HOME.
HOME = os.environ['HOME']

def GetEnvironment():
  """Get environment variables from command line and environment.

  Exits on errors.

  Returns
    Environment with the configuration from the command line.
  """
  opts = Variables()
  opts.Add('variant', 'the variant to build, optimized or optimized with output', 'optimized')
  opts.Add('program', 'program or interface to compile', 'kasvm')

  env = Environment(options=opts, ENV=os.environ)
  if not env['variant'] in ['optimized','optimized_output','debug']:
    print('Illegal value for variant: %s' % env['variant'])
    sys.exit(1)

  if not env['program'] in ['kasvm', 'single_level', 'prepare', 'knn', 'test']:
    print('Illegal value for program: %s' % env['program'])
    sys.exit(1)

  # Special configuration for 64 bit machines.
  if platform.architecture()[0] == '64bit':
     env.Append(CPPFLAGS=['-DPOINTER64=1'])

  return env

# Get the common environment.
env = GetEnvironment()

env.Append(CPPPATH=['./extern/argtable-2.10/include'])
env.Append(CPPPATH=['./extern/flann-1.8.4/include'])
env.Append(CPPPATH=['./extern/libsvm-3.22/include'])
env.Append(CPPPATH=['./extern/thundersvm/include'])
env.Append(CPPPATH=['./extern/thundersvm/build']) # need build config.h
env.Append(CPPPATH=['./extern/bayesopt/include'])
env.Append(CPPPATH=['./extern/bayesopt/utils'])
env.Append(CPPPATH=['./lib'])
env.Append(CPPPATH=['./lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/'])
env.Append(CPPPATH=['/usr/include/openmpi/'])

conf = Configure(env)

if SYSTEM == 'Darwin':
        env.Append(CPPPATH=['/opt/local/include/','../include'])
        env.Append(LIBPATH=['/opt/local/lib/'])
        env.Append(LIBPATH=['/opt/local/lib/openmpi/'])
        # homebrew related paths
        env.Append(LIBPATH=['/usr/local/lib/'])
        env.Append(LIBPATH=['/usr/local/lib/openmpi/'])
        env.Append(LIBPATH=['./extern/argtable-2.10/maclib'])
        env.Append(LIBPATH=['./extern/thundersvm/build/lib'])
        env.Append(LIBPATH=['./extern/bayesopt/build/lib'])
else:
        env.Append(LIBPATH=['./extern/argtable-2.10/lib'])
        env.Append(LIBPATH=['./extern/thundersvm/build/lib'])
        env.Append(LIBPATH=['./extern/bayesopt/build/lib'])

# bake shared library paths into executable
for lib in env["LIBPATH"]:
    env.Append(RPATH=[env.Literal(os.path.join('\\$$ORIGIN', os.pardir, lib))])

#by D. Luxen
if not conf.CheckLibWithHeader('argtable2', 'argtable2.h', 'CXX'):
        print("argtable library not found. Exiting")
        Exit(-1)
#if not conf.CheckCXXHeader('mpi.h'):
        #print("openmpi header not found. Exiting")
        #Exit(-1)

env.Append(CXXFLAGS = '-fopenmp')
if "clang" in env['CC'] or "clang" in env['CXX']:
        if env['variant'] == 'optimized':
          env.Append(CXXFLAGS = '-DNDEBUG -Wall -funroll-loops -O3 -std=c++17')
          env.Append(CCFLAGS  = '-O3  -DNDEBUG -funroll-loops -std=c++17')
        elif env['variant'] == 'optimized_output':
          # A little bit more output on the console
          env.Append(CXXFLAGS = ' -DNDEBUG -funroll-loops -Wall -O3 -std=c++17')
          env.Append(CCFLAGS  = '-O3  -DNDEBUG -DKAFFPAOUTPUT  -std=c++17')
        elif env['variant'] == 'debug':
          env.Append(CXXFLAGS = '-O0 -g3 -std=c++17')
          env.Append(CCFLAGS  = '-DKAFFPAOUTPUT -O0 -g -std=c++17')
          # env.Append(LDFLAGS  = '-pg')
          if SYSTEM != 'Darwin':
                env.Append(CXXFLAGS = '-march=native')
                env.Append(CCFLAGS  = '-march=native')

else:
        if env['variant'] == 'optimized':
          env.Append(CXXFLAGS = '-DNDEBUG -Wall -funroll-loops  -fno-stack-limit -O3 -std=c++17 -fpermissive')
          env.Append(CCFLAGS  = '-O3  -DNDEBUG -funroll-loops -std=c++17 -fpermissive')
        elif env['variant'] == 'optimized_output':
          # A little bit more output on the console
          env.Append(CXXFLAGS = ' -DNDEBUG -funroll-loops -Wall -fno-stack-limit -O3 -std=c++17 -fpermissive')
          env.Append(CCFLAGS  = '-O3  -DNDEBUG -DKAFFPAOUTPUT  -std=c++17 -fpermissive')
        elif env['variant'] == 'debug':
          env.Append(CXXFLAGS = '-O0 -g3 -std=c++17 -fpermissive')
          env.Append(CCFLAGS  = '-DKAFFPAOUTPUT -O0 -g -std=c++17')
          env.Append(LDFLAGS  = '-pg')

          if SYSTEM != 'Darwin':
                env.Append(CXXFLAGS = '-march=native')
                env.Append(CCFLAGS  = '-march=native')

Progress('$TARGET\r',
         file=open('/dev/tty', 'w'),
         overwrite=True)

# Execute the SConscript.
SConscript('SConscript', exports=['env'],variant_dir=env['variant'], duplicate=False)

