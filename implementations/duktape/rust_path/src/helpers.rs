use std::ffi::{CStr};
use std::os::raw::c_char;
use std::ffi::OsString;

pub extern fn build_str_from_c(c_path_string: *const c_char) -> String {
  unsafe {
    CStr::from_ptr(c_path_string).to_string_lossy().into_owned()
  }
}

pub extern fn path_to_str(path: OsString) -> String {
  path.to_string_lossy().into_owned()
}
