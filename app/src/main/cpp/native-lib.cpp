#include <jni.h>
#include <string>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
struct NativeWorkerArgs{
    jint id;
    jint iterations;
};
static jmethodID gOnNativeMessage = NULL;
static JavaVM* gVm=NULL;
static jobject gObj=NULL;
static pthread_mutex_t mutex;
jint JNI_OnLoad(JavaVM *vm,void *reserved){
    gVm=vm;
    return JNI_VERSION_1_4;
}

extern "C"
void Java_com_minhuizhu_thread_MainActivity_nativeInit(JNIEnv *env, jobject instance) {

   if(0!=pthread_mutex_init(&mutex,NULL)){
        jclass exceptionClazz=env->FindClass("java/lang/RuntimeException");
       env->ThrowNew(exceptionClazz,"Unable to initialize mutex");
       goto exit;
   }
    if(NULL==gObj){
        gObj=env->NewGlobalRef(instance);
        if(NULL==gObj){
            goto exit;
        }
    }
    if(NULL==gOnNativeMessage){
        jclass clazz=env->GetObjectClass(instance);
        gOnNativeMessage=env->GetMethodID(clazz,"onNativeMessage","(Ljava/lang/String;)V");
        if(NULL==gOnNativeMessage){
            jclass exceptionClazz=env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(exceptionClazz,"Unable to find method");
        }
    }
    exit:return;
}
extern "C"
void Java_com_minhuizhu_thread_MainActivity_nativeWorker(JNIEnv *env, jobject instance, jint id,
                                                    jint iterations) {

   if(0!=pthread_mutex_lock(&mutex)){
       jclass exceptionClazz=env->FindClass("java/lang/RuntimeException");
       env->ThrowNew(exceptionClazz,"Unable to lock mutex");
       goto exit;
    }
    for(jint i=0;i<iterations;i++){
        char message[26];
        sprintf(message,"Worker %d: Iteration %d",id,i);
        jstring  messageString=env->NewStringUTF(message);
        env->CallVoidMethod(instance,gOnNativeMessage,messageString);
        if(NULL!=env->ExceptionOccurred())
            break;
        sleep(1);
    }
    if(0!=pthread_mutex_unlock(&mutex)){
        jclass  exceptionClazz=env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClazz,"Unable to unlock mutex");
    }
    exit:return;
}
static void * nativeWorkerThread(void *args){
    JNIEnv *env=NULL;
    if(0==gVm->AttachCurrentThread(&env,NULL)){
        NativeWorkerArgs* nativeWorkerArgs=(NativeWorkerArgs*)args;
        Java_com_minhuizhu_thread_MainActivity_nativeWorker(env,gObj,nativeWorkerArgs->id,nativeWorkerArgs->iterations);
        delete nativeWorkerArgs;
        gVm->DetachCurrentThread();
    }
    return (void*)1;
}
extern "C"
void Java_com_minhuizhu_thread_MainActivity_posixThreads(JNIEnv *env, jobject instance, jint threads,
                                                    jint iterations) {
for(jint i = 0;i<threads;i++){
    pthread_t thread;
    NativeWorkerArgs* nativeWorkerArgs=new NativeWorkerArgs();
    nativeWorkerArgs->id=i;
    nativeWorkerArgs->iterations=iterations;
    int result =pthread_create(&thread,NULL,nativeWorkerThread,(void*)nativeWorkerArgs);
    if(0!=result){
        jclass  exceptionClazz=env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClazz,"Unable to unlock mutex");
    }
}

}
extern "C"
void Java_com_minhuizhu_thread_MainActivity_nativeFree(JNIEnv *env, jobject instance) {
if(NULL!=gObj){
    env->DeleteGlobalRef(gObj);
    gObj=NULL;
}
if(0!=pthread_mutex_destroy(&mutex)){
    jclass exceptionClazz=env->FindClass("java/lang/RuntimeException");
    env->ThrowNew(exceptionClazz,"Unable to destroy mutex");
}
}





























