require 'ffi'

module MarkersHelper
  extend FFI::Library
  ffi_lib 'c'
  ffi_lib 'http://vntech.me:8000/libgenTexData.so'
  attach_function :genTexData, [:string], :void
end
