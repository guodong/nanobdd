use std::fmt;
use std::ops::{BitAnd, BitAndAssign, BitOr, BitOrAssign, BitXor, BitXorAssign, Not, Shr, ShrAssign, Sub, SubAssign};
use std::ptr::NonNull;

#[repr(C)]
pub struct nanobdd_bdd_t {
  _private: [u8; 0],
}

extern "C" {
  fn nanobdd_init(table_size: usize, cache_size: usize, var_num: usize);
  fn nanobdd_clear();

  fn nanobdd_bdd_false() -> *mut nanobdd_bdd_t;
  fn nanobdd_bdd_true() -> *mut nanobdd_bdd_t;

  fn nanobdd_get_var(id: u32) -> *mut nanobdd_bdd_t;
  fn nanobdd_get_nvar(id: u32) -> *mut nanobdd_bdd_t;

  fn nanobdd_bdd_clone(bdd: *const nanobdd_bdd_t) -> *mut nanobdd_bdd_t;
  fn nanobdd_bdd_free(bdd: *mut nanobdd_bdd_t);

  fn nanobdd_bdd_is_false(bdd: *const nanobdd_bdd_t) -> bool;
  fn nanobdd_bdd_is_true(bdd: *const nanobdd_bdd_t) -> bool;
  fn nanobdd_bdd_eq(lhs: *const nanobdd_bdd_t, rhs: *const nanobdd_bdd_t) -> bool;

  fn nanobdd_bdd_and(lhs: *const nanobdd_bdd_t, rhs: *const nanobdd_bdd_t) -> *mut nanobdd_bdd_t;
  fn nanobdd_bdd_and_assign(lhs: *mut nanobdd_bdd_t, rhs: *const nanobdd_bdd_t);

  fn nanobdd_bdd_or(lhs: *const nanobdd_bdd_t, rhs: *const nanobdd_bdd_t) -> *mut nanobdd_bdd_t;
  fn nanobdd_bdd_or_assign(lhs: *mut nanobdd_bdd_t, rhs: *const nanobdd_bdd_t);

  fn nanobdd_bdd_not(value: *const nanobdd_bdd_t) -> *mut nanobdd_bdd_t;

  fn nanobdd_bdd_xor(lhs: *const nanobdd_bdd_t, rhs: *const nanobdd_bdd_t) -> *mut nanobdd_bdd_t;
  fn nanobdd_bdd_xor_assign(lhs: *mut nanobdd_bdd_t, rhs: *const nanobdd_bdd_t);

  fn nanobdd_bdd_diff(lhs: *const nanobdd_bdd_t, rhs: *const nanobdd_bdd_t) -> *mut nanobdd_bdd_t;
  fn nanobdd_bdd_diff_assign(lhs: *mut nanobdd_bdd_t, rhs: *const nanobdd_bdd_t);

  fn nanobdd_bdd_imp(lhs: *const nanobdd_bdd_t, rhs: *const nanobdd_bdd_t) -> *mut nanobdd_bdd_t;
  fn nanobdd_bdd_imp_assign(lhs: *mut nanobdd_bdd_t, rhs: *const nanobdd_bdd_t);

  fn nanobdd_num_nodes() -> usize;
  fn nanobdd_gc();
  fn nanobdd_debug_nodes();
}

pub fn init(table_size: usize, cache_size: usize, var_num: usize) {
  unsafe { nanobdd_init(table_size, cache_size, var_num) };
}

/// # Safety
/// Calling this invalidates all existing Bdd values. Ensure no Bdd is in use.
pub unsafe fn clear() {
  nanobdd_clear();
}

#[derive(Eq)]
pub struct Bdd {
  raw: NonNull<nanobdd_bdd_t>,
}

impl Bdd {
  fn from_raw(ptr: *mut nanobdd_bdd_t) -> Self {
    let raw = NonNull::new(ptr).expect("nanobdd returned null BDD pointer");
    Self { raw }
  }

  pub unsafe fn from_raw_unchecked(ptr: *mut nanobdd_bdd_t) -> Self {
    Self { raw: NonNull::new_unchecked(ptr) }
  }

  pub fn as_raw(&self) -> *mut nanobdd_bdd_t {
    self.raw.as_ptr()
  }

  pub fn is_false(&self) -> bool {
    unsafe { nanobdd_bdd_is_false(self.raw.as_ptr()) }
  }

  pub fn is_true(&self) -> bool {
    unsafe { nanobdd_bdd_is_true(self.raw.as_ptr()) }
  }

  pub fn and(&self, rhs: &Bdd) -> Bdd {
    unsafe { Bdd::from_raw(nanobdd_bdd_and(self.raw.as_ptr(), rhs.raw.as_ptr())) }
  }

  pub fn and_assign(&mut self, rhs: &Bdd) {
    unsafe { nanobdd_bdd_and_assign(self.raw.as_ptr(), rhs.raw.as_ptr()) };
  }

  pub fn or(&self, rhs: &Bdd) -> Bdd {
    unsafe { Bdd::from_raw(nanobdd_bdd_or(self.raw.as_ptr(), rhs.raw.as_ptr())) }
  }

  pub fn or_assign(&mut self, rhs: &Bdd) {
    unsafe { nanobdd_bdd_or_assign(self.raw.as_ptr(), rhs.raw.as_ptr()) };
  }

  pub fn not(&self) -> Bdd {
    unsafe { Bdd::from_raw(nanobdd_bdd_not(self.raw.as_ptr())) }
  }

  pub fn xor(&self, rhs: &Bdd) -> Bdd {
    unsafe { Bdd::from_raw(nanobdd_bdd_xor(self.raw.as_ptr(), rhs.raw.as_ptr())) }
  }

  pub fn xor_assign(&mut self, rhs: &Bdd) {
    unsafe { nanobdd_bdd_xor_assign(self.raw.as_ptr(), rhs.raw.as_ptr()) };
  }

  pub fn diff(&self, rhs: &Bdd) -> Bdd {
    unsafe { Bdd::from_raw(nanobdd_bdd_diff(self.raw.as_ptr(), rhs.raw.as_ptr())) }
  }

  pub fn diff_assign(&mut self, rhs: &Bdd) {
    unsafe { nanobdd_bdd_diff_assign(self.raw.as_ptr(), rhs.raw.as_ptr()) };
  }

  pub fn imp(&self, rhs: &Bdd) -> Bdd {
    unsafe { Bdd::from_raw(nanobdd_bdd_imp(self.raw.as_ptr(), rhs.raw.as_ptr())) }
  }

  pub fn imp_assign(&mut self, rhs: &Bdd) {
    unsafe { nanobdd_bdd_imp_assign(self.raw.as_ptr(), rhs.raw.as_ptr()) };
  }
}

unsafe impl Send for Bdd {}
unsafe impl Sync for Bdd {}

impl Clone for Bdd {
  fn clone(&self) -> Self {
    unsafe { Bdd::from_raw(nanobdd_bdd_clone(self.raw.as_ptr())) }
  }
}

impl Drop for Bdd {
  fn drop(&mut self) {
    unsafe { nanobdd_bdd_free(self.raw.as_ptr()) };
  }
}

impl PartialEq for Bdd {
  fn eq(&self, other: &Self) -> bool {
    unsafe { nanobdd_bdd_eq(self.raw.as_ptr(), other.raw.as_ptr()) }
  }
}

impl fmt::Debug for Bdd {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "Bdd({:p})", self.raw)
  }
}

impl<'a, 'b> BitAnd<&'b Bdd> for &'a Bdd {
  type Output = Bdd;

  fn bitand(self, rhs: &'b Bdd) -> Self::Output {
    self.and(rhs)
  }
}

impl BitAndAssign<&Bdd> for Bdd {
  fn bitand_assign(&mut self, rhs: &Bdd) {
    self.and_assign(rhs);
  }
}

impl<'a, 'b> BitOr<&'b Bdd> for &'a Bdd {
  type Output = Bdd;

  fn bitor(self, rhs: &'b Bdd) -> Self::Output {
    self.or(rhs)
  }
}

impl BitOrAssign<&Bdd> for Bdd {
  fn bitor_assign(&mut self, rhs: &Bdd) {
    self.or_assign(rhs);
  }
}

impl<'a> Not for &'a Bdd {
  type Output = Bdd;

  fn not(self) -> Self::Output {
    self.not()
  }
}

impl<'a, 'b> BitXor<&'b Bdd> for &'a Bdd {
  type Output = Bdd;

  fn bitxor(self, rhs: &'b Bdd) -> Self::Output {
    self.xor(rhs)
  }
}

impl BitXorAssign<&Bdd> for Bdd {
  fn bitxor_assign(&mut self, rhs: &Bdd) {
    self.xor_assign(rhs);
  }
}

impl<'a, 'b> Sub<&'b Bdd> for &'a Bdd {
  type Output = Bdd;

  fn sub(self, rhs: &'b Bdd) -> Self::Output {
    self.diff(rhs)
  }
}

impl SubAssign<&Bdd> for Bdd {
  fn sub_assign(&mut self, rhs: &Bdd) {
    self.diff_assign(rhs);
  }
}

impl<'a, 'b> Shr<&'b Bdd> for &'a Bdd {
  type Output = Bdd;

  fn shr(self, rhs: &'b Bdd) -> Self::Output {
    self.imp(rhs)
  }
}

impl ShrAssign<&Bdd> for Bdd {
  fn shr_assign(&mut self, rhs: &Bdd) {
    self.imp_assign(rhs);
  }
}

pub fn bdd_false() -> Bdd {
  unsafe { Bdd::from_raw(nanobdd_bdd_false()) }
}

pub fn bdd_true() -> Bdd {
  unsafe { Bdd::from_raw(nanobdd_bdd_true()) }
}

pub fn var(id: u32) -> Bdd {
  unsafe { Bdd::from_raw(nanobdd_get_var(id)) }
}

pub fn nvar(id: u32) -> Bdd {
  unsafe { Bdd::from_raw(nanobdd_get_nvar(id)) }
}

pub fn num_nodes() -> usize {
  unsafe { nanobdd_num_nodes() }
}

pub fn gc() {
  unsafe { nanobdd_gc() };
}

pub fn debug_nodes() {
  unsafe { nanobdd_debug_nodes() };
}
