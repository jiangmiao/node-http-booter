srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.env.append_unique('CXXFLAGS', ['-Wall', '-O3', '-std=c++0x', '-g'])
  conf.check_tool('node_addon')

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'http-booter'
  obj.source = [
    'src/utils.cc',
    'src/http.cc', 
    'src/buffer.cc',
    'src/http-booter.cc',
    'src/node-http-booter.cc'
  ]
