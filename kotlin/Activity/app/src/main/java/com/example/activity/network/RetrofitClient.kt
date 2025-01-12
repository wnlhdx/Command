package com.example.activity.network

import okhttp3.OkHttpClient
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import java.util.concurrent.TimeUnit

object RetrofitClient {
    private const val BASE_URL = "http://192.168.1.3:8080"

    // 创建 OkHttpClient，并配置超时
    private val okHttpClient = OkHttpClient.Builder()
        .connectTimeout(30, TimeUnit.SECONDS)  // 连接超时
        .readTimeout(60, TimeUnit.SECONDS)     // 读取超时
        .writeTimeout(60, TimeUnit.SECONDS)    // 写入超时
        .build()

    val apiService: ApiService by lazy {
        Retrofit.Builder()
            .baseUrl(BASE_URL)
            .client(okHttpClient)  // 使用自定义的 OkHttpClient
            .addConverterFactory(GsonConverterFactory.create())
            .build()
            .create(ApiService::class.java)
    }
}
