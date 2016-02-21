require 'ffi'

module MarkersHelper
  extend FFI::Library
  ffi_lib 'c'
  ffi_lib 'libgenTexData.so'
  attach_function :genTexData, [:string], :void
end
