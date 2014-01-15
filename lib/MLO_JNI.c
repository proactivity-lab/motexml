#include <jni.h>
#include <stdlib.h>

#include "MLO_JNI.h"
#include <MLO.h>

#include <android/log.h>

const char* MLONAME = "mlo";

jlong Java_com_defendec_subsserver_MLO_create(JNIEnv* env, jobject thiz)
{
    ml_object_t* obj = (ml_object_t*) malloc(sizeof(ml_object_t));
    obj->buffer = NULL;
    obj->bufferLength = 0;
    obj->index = 0;
    obj->subject = 0;
    obj->type = 0;
    obj->value = 0;
    obj->valueIsPresent = FALSE;
    return (jlong) (intptr_t) obj;
}

void Java_com_defendec_subsserver_MLO_destroy(JNIEnv* env, jobject thiz, jlong object)
{
    ml_object_t* obj = (ml_object_t*) (intptr_t) object;
    if (obj != NULL)
    {
        if (obj->buffer != NULL)
        {
            free(obj->buffer);
            obj->buffer = NULL;
        }
        free(obj);
    }
}

jshort Java_com_defendec_subsserver_MLO_MLO_1getIndex(JNIEnv* env, jobject thiz, jlong object)
{
    return (jshort) MLO_getIndex((ml_object_t*) (intptr_t) object);
}

jlong Java_com_defendec_subsserver_MLO_MLO_1getType(JNIEnv* env, jobject thiz, jlong object)
{
    return (jlong) MLO_getType((ml_object_t*) (intptr_t) object);
}

jint Java_com_defendec_subsserver_MLO_MLO_1getValue(JNIEnv* env, jobject thiz, jlong object)
{
    return (jint) MLO_getValue((ml_object_t*) (intptr_t) object);
}

jboolean Java_com_defendec_subsserver_MLO_MLO_1getValueIsPresent(JNIEnv* env, jobject thiz, jlong object)
{
    return (jboolean) MLO_getValueIsPresent((ml_object_t*) (intptr_t) object);
}

jshort Java_com_defendec_subsserver_MLO_MLO_1getSubject(JNIEnv* env, jobject thiz, jlong object)
{
    return (jshort) MLO_getSubject((ml_object_t*) (intptr_t) object);
}

jbyteArray Java_com_defendec_subsserver_MLO_MLO_1getBuffer(JNIEnv* env, jobject thiz, jlong object)
{
    ml_object_t* obj = (ml_object_t*) (intptr_t) object;
    uint8_t len = MLO_getBufferLength(obj);
    uint8_t* buf = MLO_getBuffer(obj);
    jbyte* jbuf = (jbyte*) malloc(len);
    memcpy(jbuf, buf, len);
    jbyteArray res = (*env)->NewByteArray(env, len);
    (*env)->SetByteArrayRegion(env, res, 0, len, jbuf);
    free(jbuf);
    return res;
}

void Java_com_defendec_subsserver_MLO_MLO_1setIndex(JNIEnv* env, jobject thiz, jlong object, jshort ndex)
{
    MLO_setIndex((ml_object_t*) (intptr_t) object, (uint8_t) ndex);
}

void Java_com_defendec_subsserver_MLO_MLO_1setType(JNIEnv* env, jobject thiz, jlong object, jlong type)
{
    MLO_setType((ml_object_t*) (intptr_t) object, (uint32_t) type);
}

void Java_com_defendec_subsserver_MLO_MLO_1setValue(JNIEnv* env, jobject thiz, jlong object, jint value)
{
    MLO_setValue((ml_object_t*) (intptr_t) object, (int32_t) value);
}

void Java_com_defendec_subsserver_MLO_MLO_1setValueIsPresent(JNIEnv* env, jobject thiz, jlong object, jboolean present)
{
    MLO_setValueIsPresent((ml_object_t*) (intptr_t) object, (bool) present);
}

void Java_com_defendec_subsserver_MLO_MLO_1setSubject(JNIEnv* env, jobject thiz, jlong object, jshort subject)
{
    MLO_setSubject((ml_object_t*) (intptr_t) object, (jshort) subject);
}

void Java_com_defendec_subsserver_MLO_MLO_1setBuffer(JNIEnv* env, jobject thiz, jlong object, jbyteArray buffer)
{
    ml_object_t* obj = (ml_object_t*) (intptr_t) object;
    jbyte* jbuf = (*env)->GetByteArrayElements(env, buffer, NULL);
    jsize len = (*env)->GetArrayLength(env, buffer);
    if (len > 0)
    {
        uint8_t* buf = (uint8_t*) malloc(len);
        memcpy(buf, jbuf, len);
        MLO_setBuffer(obj, buf, len);
    }
    (*env)->ReleaseByteArrayElements(env, buffer, jbuf, JNI_ABORT);
}
