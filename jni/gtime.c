
#include <android/log.h>
#include <jni.h>
#include <strings.h>

#include "rtklib.h"

#define TAG "nativeGTime"
#define LOGV(...) showmsg(__VA_ARGS__)

static struct {
   jfieldID time;
   jfieldID sec;
} ru0xdc_rtklib_gtime_fields;

static struct {
   jmethodID set_gtime;
} ru0xdc_rtklib_gtime_methods;


static gtime_t get_gtime_t(JNIEnv* env, jobject thiz)
{
   gtime_t gt;
   gt.time = (*env)->GetLongField(env, thiz, ru0xdc_rtklib_gtime_fields.time);
   gt.sec = (*env)->GetDoubleField(env, thiz, ru0xdc_rtklib_gtime_fields.sec);
   return gt;
}

static jlong GTime_get_utc_time_millis(JNIEnv* env, jobject thiz)
{
   gtime_t t;
   t = gpst2utc(get_gtime_t(env, thiz));
   return (jlong)(1000ll * t.time + llround(1000.0 * t.sec));
}

static jint GTime_get_gps_week(JNIEnv* env, jobject thiz)
{
   int week;
   time2gpst(get_gtime_t(env, thiz), &week);
   return week;
}

static jdouble GTime_get_gps_tow(JNIEnv* env, jobject thiz)
{
   return time2gpst(get_gtime_t(env, thiz), NULL);
}

static int init_gtime_fields_methods(JNIEnv* env, jclass clazz) {

    ru0xdc_rtklib_gtime_fields.time = (*env)->GetFieldID(env, clazz, "time", "J");
    ru0xdc_rtklib_gtime_fields.sec = (*env)->GetFieldID(env, clazz, "sec", "D");
    ru0xdc_rtklib_gtime_methods.set_gtime = (*env)->GetMethodID(env, clazz, "setGTime", "(JD)V");

    if ((ru0xdc_rtklib_gtime_fields.time == NULL)
	  || (ru0xdc_rtklib_gtime_fields.sec == NULL)
	  || (ru0xdc_rtklib_gtime_methods.set_gtime == NULL)
	  ) {
       LOGV("ru0xdc/rtklib/GTime fields not found:%s%s%s",
	     (ru0xdc_rtklib_gtime_fields.time == NULL ? " time" : ""),
	     (ru0xdc_rtklib_gtime_fields.sec == NULL ? " sec" : ""),
	     (ru0xdc_rtklib_gtime_methods.set_gtime == NULL ? " setGTime" : "")
	     );
       return JNI_FALSE;
    }

    return JNI_TRUE;
}

void set_gtime(JNIEnv* env, jclass jgtime, gtime_t time)
{
   (*env)->CallVoidMethod(env, jgtime, ru0xdc_rtklib_gtime_methods.set_gtime,
	 (jlong)time.time,
	 (jdouble)time.sec);
}


static JNINativeMethod nativeMethods[] = {
   { "getUtcTimeMillis", "()J", (void*)GTime_get_utc_time_millis },
   { "getGpsWeek", "()I", (void*)GTime_get_gps_week },
   { "getGpsTow", "()D", (void*)GTime_get_gps_tow }
};

int registerGTimeNatives(JNIEnv* env) {
    int result = -1;

    /* look up the class */
    jclass clazz = (*env)->FindClass(env, "ru0xdc/rtklib/GTime");

    if (clazz == NULL)
       return JNI_FALSE;

    if ((*env)->RegisterNatives(env, clazz, nativeMethods, sizeof(nativeMethods)
	     / sizeof(nativeMethods[0])) != JNI_OK)
       return JNI_FALSE;

    if ( ! init_gtime_fields_methods(env, clazz))
       return JNI_FALSE;

    return JNI_TRUE;
}

