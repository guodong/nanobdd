use std::env;
use std::path::PathBuf;

fn main() {
  let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR"));
  let root_dir = manifest_dir
    .parent()
    .expect("rust/ should live under the repo root")
    .to_path_buf();

  let src_dir = root_dir.join("src");
  let include_dir = root_dir.join("include");

  let tbb = pkg_config::Config::new().probe("tbb").ok();

  let mut build = cc::Build::new();
  if let Some(lib) = &tbb {
    for path in &lib.include_paths {
      build.include(path);
    }
  }
  build
    .cpp(true)
    .flag_if_supported("-std=c++17")
    .flag_if_supported("-fPIC")
    .include(&src_dir)
    .include(&include_dir)
    .file(root_dir.join("src/Bdd.cpp"))
    .file(root_dir.join("src/Cache.cpp"))
    .file(root_dir.join("src/Nanobdd.cpp"))
    .file(root_dir.join("src/NodeAllocator.cpp"))
    .file(root_dir.join("src/NodeTable.cpp"))
    .file(root_dir.join("src/capi.cpp"));

  build.compile("nanobdd");

  println!("cargo:rerun-if-changed={}", root_dir.join("src/Bdd.cpp").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("src/Cache.cpp").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("src/Nanobdd.cpp").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("src/NodeAllocator.cpp").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("src/NodeTable.cpp").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("src/capi.cpp").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("include/nanobdd/Bdd.h").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("include/nanobdd/Node.h").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("include/nanobdd/nanobdd.h").display());
  println!("cargo:rerun-if-changed={}", root_dir.join("include/nanobdd/capi.h").display());

  let target = env::var("TARGET").unwrap_or_default();
  if target.contains("apple") {
    println!("cargo:rustc-link-lib=c++");
  } else if target.contains("windows") {
    // MSVC links the C++ standard library automatically.
  } else {
    println!("cargo:rustc-link-lib=stdc++");
  }

  if let Some(lib) = tbb {
    for path in lib.include_paths {
      println!("cargo:include={}", path.display());
    }
  }
}
