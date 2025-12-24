use std::sync::Mutex;

use nanobdd::{gc, init, num_nodes, var};

static TEST_MUTEX: Mutex<()> = Mutex::new(());

#[test]
fn node_table_slab_growth_and_gc() {
  let _guard = TEST_MUTEX.lock().expect("test mutex poisoned");

  // Ensure a clean global state before starting.
  unsafe { nanobdd::clear() };

  const TABLE_SIZE: usize = 5;
  const CACHE_SIZE: usize = 5;
  const VAR_NUM: usize = 3;

  init(TABLE_SIZE, CACHE_SIZE, VAR_NUM);

  let baseline = num_nodes();

  // Trigger many unique nodes to push past the initial slab capacity and hash
  // bucket count. If growth fails, we'd crash or get null derefs.
  let mut arena = Vec::with_capacity(200);
  for i in 0..200 {
    let a = var((i % VAR_NUM) as u32);
    let b = var(((i + 1) % VAR_NUM) as u32);
    let c = var(((i + 2) % VAR_NUM) as u32);
    arena.push(&(&a ^ &b) | &(&a & &c));
  }

  assert!(
    num_nodes() > baseline + 10,
    "slab growth / maintenance path should allow many allocations"
  );

  // Release BDDs so their roots are deref'ed, then run GC to reclaim.
  arena.clear();
  gc();

  let after_gc = num_nodes();
  let max_reachable = 2 + 2 * VAR_NUM; // terminals + vars + nvars
  assert!(
    after_gc == max_reachable,
    "GC should reclaim unreachable nodes into free list"
  );

  // Allocate again; should succeed via free list without unbounded growth.
  let before_alloc = num_nodes();
  let d = {
    let v0 = var(0);
    let v1 = var(1);
    &v0 ^ &v1
  };
  drop(d);
  assert!(
    num_nodes() == before_alloc + 1,
    "re-allocation should use reclaimed nodes or a small growth"
  );

  unsafe { nanobdd::clear() };
}
