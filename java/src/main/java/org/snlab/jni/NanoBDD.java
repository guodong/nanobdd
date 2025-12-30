package org.snlab.jni;

public class NanoBDD {
    private final long cxxThis;
    private boolean closed = false;

    public NanoBDD(int tableSize, int cacheSize, int numVar) {
        cxxThis = cxxConstruct(tableSize, cacheSize, numVar);
    };

    public void close() {
       if(!closed) {
           cxxDestroy(cxxThis);
           closed = true;
       }
    }

    @Deprecated
    protected void finalize() {
       close();
    }

    private static native long cxxConstruct(int tableSize, int cacheSize, int numVar);
    private static native void cxxDestroy(long cxxThis);

    public native long var(int idx);

    public native long nvar(int idx);

    public native void clear();

    public native long getTrue();

    public native long getFalse();

    public native long not(long a);

    public native long and(long a, long b);

    public native long or(long a, long b);

    public native long diff(long a, long b);

    public native void ref(long a);

    public native void deRef(long a);

    public native void gc();

    public native long nodeCount();

    static {
        System.load("/usr/local/lib/libnanobdd.so");
    }

    public static void main(String[] args) {
        NanoBDD bddEngine = new NanoBDD(3, 10, 3);

        long x = bddEngine.var(0);
        long y = bddEngine.var(1);
        long z = bddEngine.var(2);

        long xy = bddEngine.and(x, y);
        long xyz = bddEngine.and(xy, z);
        long xyZ = bddEngine.and(xy, bddEngine.not(z));

        assert xy == bddEngine.or(xyz, xyZ);
        assert xy != bddEngine.getFalse();

        System.out.println("Node count: " + bddEngine.nodeCount());

        bddEngine.gc();

        System.out.println("Node count after gc: " + bddEngine.nodeCount());
    }
}
