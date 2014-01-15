#include <jni.h>
#include <stdlib.h>
#include "MLE_JNI.h"
#include <MLE.h>

#include <android/log.h>

const char* MLENAME = "mle";

jlong Java_com_defendec_subsserver_MLE_create(JNIEnv* env, jobject thiz, jint bufferSize)
{
    ml_encoder_t* enc = (ml_encoder_t*) malloc(sizeof(ml_encoder_t));
    uint8_t* buf = (uint8_t*) malloc(bufferSize);
    int res = MLE_initialize(enc, buf, bufferSize);
    if (res != SUCCESS)
    {
        Java_com_defendec_subsserver_MLE_destroy(env, thiz, (jlong) (intptr_t) enc);
        enc = NULL;
    }
    return (jlong) (intptr_t) enc;
}

void Java_com_defendec_subsserver_MLE_destroy(JNIEnv* env, jobject thiz, jlong enc)
{
    ml_encoder_t* obj = (ml_encoder_t*) (intptr_t) enc;
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

jbyteArray Java_com_defendec_subsserver_MLE_MLE_1getEncoded(JNIEnv* env, jobject thiz, jlong enc)
{
    jbyteArray res = NULL;

    ml_encoder_t* obj = (ml_encoder_t*) (intptr_t) enc;
    uint8_t* buf = obj->buf;
    if (buf == NULL)
    {
        return res;
    }
    uint8_t len = MLE_finalize(obj);
    if (len > 0)
    {
        jbyte* jbuf = (jbyte*) malloc(len);
        memcpy(jbuf, buf, len);
        res = (*env)->NewByteArray(env, len);
        (*env)->SetByteArrayRegion(env, res, 0, len, jbuf);
        free(jbuf);
    }
    return res;
}

jshort Java_com_defendec_subsserver_MLE_MLE_1appendObject(JNIEnv* env, jobject thiz, jlong enc, jlong object)
{
    return (jshort) MLE_appendObject((ml_encoder_t*) (intptr_t) enc, (ml_object_t*) (intptr_t) object);
}

jshort Java_com_defendec_subsserver_MLE_MLE_1appendOSV(JNIEnv* env, jobject thiz, jlong enc, jlong type, jshort subject,
        jint value)
{
    return (jshort) MLE_appendOSV((ml_encoder_t*) (intptr_t) enc, (uint32_t) type, (uint8_t) subject, (int) value);
}

jshort Java_com_defendec_subsserver_MLE_MLE_1appendOV(JNIEnv* env, jobject thiz, jlong enc, jlong type, jint value)
{
    return (jshort) MLE_appendOV((ml_encoder_t*) (intptr_t) enc, (uint32_t) type, (int) value);
}

jshort Java_com_defendec_subsserver_MLE_MLE_1appendOS(JNIEnv* env, jobject thiz, jlong enc, jlong type, jshort subject)
{
    return (jshort) MLE_appendOS((ml_encoder_t*) (intptr_t) enc, (uint32_t) type, (uint8_t) subject);
}

jshort Java_com_defendec_subsserver_MLE_MLE_1appendO(JNIEnv* env, jobject thiz, jlong enc, jlong type)
{
    return (jshort) MLE_appendO((ml_encoder_t*) (intptr_t) enc, (uint32_t) type);
}

jshort Java_com_defendec_subsserver_MLE_MLE_1spaceLeft(JNIEnv* env, jobject thiz, jlong enc)
{
    return (jshort) MLE_spaceLeft((ml_encoder_t*) (intptr_t) enc);
}

jshort Java_com_defendec_subsserver_MLE_MLE_1spaceUsed(JNIEnv* env, jobject thiz, jlong enc)
{
    return (jshort) MLE_spaceUsed((ml_encoder_t*) (intptr_t) enc);
}

jshort Java_com_defendec_subsserver_MLE_MLE_1deleteObject(JNIEnv* env, jobject thiz, jlong enc, jshort number)
{
    return (jshort) MLE_deleteObject((ml_encoder_t*) (intptr_t) enc, (uint8_t) number);
}

