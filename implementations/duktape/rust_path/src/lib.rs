use std::ffi::CString;
use std::os::raw::c_char;
use std::path::Path;

mod helpers;

#[no_mangle]
pub extern fn rust_join(c_path_string1: *const c_char,
                            c_path_string2: *const c_char)
                            -> *const c_char {
    helpers::protect_against_panic(|| {
        let path_string1 = helpers::build_str_from_c(c_path_string1);
        let path_string2 = helpers::build_str_from_c(c_path_string2);
        let path1 = Path::new(&path_string1);
        let joined_path = path1.join(path_string2);

        let joined_string = helpers::path_to_str(joined_path.into_os_string());
        let c_joined = CString::new(joined_string).unwrap();
        c_joined.into_raw()
    })
}

#[no_mangle]
pub extern fn rust_dirname(c_path_string: *const c_char) -> *const c_char {
    helpers::protect_against_panic(|| {
        let path_string = helpers::build_str_from_c(c_path_string);
        let path = Path::new(&path_string);
        let dirname_string =
            helpers::path_to_str(path.parent().unwrap().to_owned().into_os_string());
        let c_dirname = CString::new(dirname_string).unwrap();
        c_dirname.into_raw()
    })
}

#[no_mangle]
pub extern fn rust_extension(c_path_string: *const c_char) -> *const c_char {
    helpers::protect_against_panic(|| {
        let path_string = helpers::build_str_from_c(c_path_string);
        let path = Path::new(&path_string);
        let ext = path.extension().unwrap();
        CString::new(helpers::path_to_str(ext.to_owned())).unwrap().into_raw()
    })
}

#[no_mangle]
pub extern fn rust_filename(c_path_string: *const c_char) -> *const c_char {
    helpers::protect_against_panic(|| {
        let path_string = helpers::build_str_from_c(c_path_string);
        let path = Path::new(&path_string);
        let filename_string = helpers::path_to_str(path.file_name().unwrap().to_owned());
        let c_filename = CString::new(filename_string).unwrap();
        c_filename.into_raw()
    })
}
