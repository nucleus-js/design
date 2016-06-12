use std::ffi::CStr;
use std::os::raw::c_char;
use std::ffi::OsString;
use std::ptr;
use std::panic;

pub fn build_str_from_c(c_path_string: *const c_char) -> String {
    unsafe { CStr::from_ptr(c_path_string).to_string_lossy().into_owned() }
}

pub fn path_to_str(path: OsString) -> String {
    path.to_string_lossy().into_owned()
}

pub fn protect_against_panic<F>(code: F) -> *const c_char
    where F: Fn() -> *const c_char + panic::UnwindSafe
{
    let result = panic::catch_unwind(code);

    match result {
        Ok(value) => value,
        Err(_) => ptr::null(),
    }
}
