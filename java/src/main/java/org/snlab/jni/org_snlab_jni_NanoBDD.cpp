#include "org_snlab_jni_NanoBDD.h"
#include "nanobdd/Bdd.h"
#include "nanobdd/Node.h"
#include "nanobdd/nanobdd.h"
#include <jni.h>

using namespace nanobdd;

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_cxxConstruct(
    JNIEnv *env, jclass cls, jint table_size, jint cache_size, jint var_num) {
  // Since nanobdd is a singleton, we don't have a pointer to a nanobdd
  // object.
  nanobdd::init((size_t)table_size, (size_t)cache_size, (size_t)var_num);
  return (jlong) nullptr;
}

JNIEXPORT void JNICALL Java_org_snlab_jni_NanoBDD_cxxDestroy(JNIEnv *, jclass,
                                                             jlong) {
  nanobdd::clear();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_var(JNIEnv *env, jobject obj,
                                                       jint idx) {
  return (jlong)nanobdd::getVar((uint32_t)idx).root();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_nvar(JNIEnv *env,
                                                        jobject obj, jint idx) {
  return (jlong)nanobdd::getNvar((uint32_t)idx).root();
}

JNIEXPORT void JNICALL Java_org_snlab_jni_NanoBDD_clear(JNIEnv *env,
                                                        jobject obj) {
  nanobdd::clear();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_getTrue(JNIEnv *env,
                                                           jobject obj) {
  return (jlong)nanobdd::bddTrue().root();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_getFalse(JNIEnv *env,
                                                            jobject obj) {
  return (jlong)nanobdd::bddFalse().root();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_not(JNIEnv *env, jobject obj,
                                                       jlong x) {
  auto x_bdd = nanobdd::Bdd((nanobdd::Node *)x);
  return (jlong)(!x_bdd).root();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_and(JNIEnv *env, jobject obj,
                                                       jlong x, jlong y) {
  auto x_bdd = nanobdd::Bdd((nanobdd::Node *)x);
  auto y_bdd = nanobdd::Bdd((nanobdd::Node *)y);
  return (jlong)(x_bdd & y_bdd).root();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_or(JNIEnv *env, jobject obj,
                                                      jlong x, jlong y) {
  auto x_bdd = nanobdd::Bdd((nanobdd::Node *)x);
  auto y_bdd = nanobdd::Bdd((nanobdd::Node *)y);
  return (jlong)(x_bdd | y_bdd).root();
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_diff(JNIEnv *env,
                                                        jobject obj, jlong x,
                                                        jlong y) {
  auto x_bdd = nanobdd::Bdd((nanobdd::Node *)x);
  auto y_bdd = nanobdd::Bdd((nanobdd::Node *)y);
  return (jlong)(x_bdd - y_bdd).root();
}

JNIEXPORT void JNICALL Java_org_snlab_jni_NanoBDD_ref(JNIEnv *env, jobject obj,
                                                      jlong x) {
  ((nanobdd::Node *)x)->ref();
  return;
}

JNIEXPORT void JNICALL Java_org_snlab_jni_NanoBDD_deRef(JNIEnv *env,
                                                        jobject obj, jlong x) {
  ((nanobdd::Node *)x)->deref();
  return;
}

JNIEXPORT void JNICALL Java_org_snlab_jni_NanoBDD_gc(JNIEnv *env, jobject obj) {
  nanobdd::gc();
  return;
}

JNIEXPORT jlong JNICALL Java_org_snlab_jni_NanoBDD_nodeCount(JNIEnv *env,
                                                             jobject obj) {
  return (jlong)nanobdd::numNodes();
}
