#include <jni.h>
#include <stdlib.h>
#include "MLI_JNI.h"
#include <MLI.h>
#include <MLO.h>

#include <android/log.h>

const char* MLINAME = "mli";

const char* MLO_CLASS_NAME = "com/defendec/subsserver/MLO";

jlong Java_com_defendec_subsserver_MLI_create(JNIEnv* env, jobject thiz, jbyteArray buffer)
{
    jbyte* jbuf = (*env)->GetByteArrayElements(env, buffer, NULL);
    jsize len = (*env)->GetArrayLength(env, buffer);
    uint8_t* buf = (uint8_t*) malloc(len);
    memcpy(buf, jbuf, len);
    (*env)->ReleaseByteArrayElements(env, buffer, jbuf, JNI_ABORT);

    ml_iterator_t* iter = (ml_iterator_t*) malloc(sizeof(ml_iterator_t));
    int res = MLI_initialize(iter, buf, (uint8_t) len);
    if (res != SUCCESS)
    {
        Java_com_defendec_subsserver_MLI_destroy(env, thiz, (jlong) (intptr_t) iter);
        iter = NULL;
    }
    return (jlong) (intptr_t) iter;
}

void Java_com_defendec_subsserver_MLI_destroy(JNIEnv* env, jobject thiz, jlong iter)
{
    ml_iterator_t* obj = (ml_iterator_t*) (intptr_t) iter;
    if (obj != NULL)
    {
        if (obj->buf != NULL)
        {
            free(obj->buf);
            obj->buf = NULL;
        }
        free(obj);
    }
}

jint Java_com_defendec_subsserver_MLI_MLE_1reset(JNIEnv* env, jobject thiz, jlong iter)
{
    return (jint) MLI_reset((ml_iterator_t*) (intptr_t) iter);
}

jobject Java_com_defendec_subsserver_MLI_MLE_1next(JNIEnv* env, jobject thiz, jlong iter)
{
    jobject mlo = NULL;
    jclass cls = (*env)->FindClass(env, MLO_CLASS_NAME);
    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "()V");
    jmethodID getObject = (*env)->GetMethodID(env, cls, "getObject", "()J");
    if (constructor == NULL || getObject == NULL)
    {
        return mlo;
    }
    mlo = (*env)->NewObject(env, cls, constructor);
    ml_object_t* obj = (ml_object_t*) (intptr_t) (*env)->CallLongMethod(env, mlo, getObject);
    if (obj != NULL)
    {
        uint8_t ndex = MLI_next((ml_iterator_t*) (intptr_t) iter, obj);
        uint8_t* tempBuffer = obj->buffer;
        obj->buffer = (uint8_t*) malloc(obj->bufferLength);
        memcpy(obj->buffer, tempBuffer, obj->bufferLength);
        if (ndex <= 0)
        {
            mlo = NULL;
        }
    }
    return mlo;
}

jobject Java_com_defendec_subsserver_MLI_MLI_1nextWithSubject(JNIEnv* env, jobject thiz, jlong iter, jshort subject)
{
    jobject mlo = NULL;
    jclass cls = (*env)->FindClass(env, MLO_CLASS_NAME);
    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "()V");
    jmethodID getObject = (*env)->GetMethodID(env, cls, "getObject", "()J");
    if (constructor == NULL || getObject == NULL)
    {
        return mlo;
    }
    mlo = (*env)->NewObject(env, cls, constructor);
    ml_object_t* obj = (ml_object_t*) (intptr_t) (*env)->CallLongMethod(env, mlo, getObject);
    if (obj != NULL)
    {
        uint8_t ndex = MLI_nextWithSubject((ml_iterator_t*) (intptr_t) iter, subject, obj);
        uint8_t* tempBuffer = obj->buffer;
        obj->buffer = (uint8_t*) malloc(obj->bufferLength);
        memcpy(obj->buffer, tempBuffer, obj->bufferLength);
        if (ndex <= 0)
        {
            mlo = NULL;
        }
    }
    return mlo;
}
