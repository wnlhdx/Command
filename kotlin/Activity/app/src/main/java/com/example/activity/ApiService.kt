package com.example.nextjsapp.network

import com.example.nextjsapp.model.Book
import com.example.nextjsapp.model.Plan
import retrofit2.http.*

interface ApiService {

    // Plan 相关接口
    @GET("/plans")
    suspend fun getAllPlans(): List<Plan>

    @POST("/plans")
    suspend fun createPlan(@Body plan: Plan): Plan

    @PUT("/plans/{id}")
    suspend fun updatePlan(@Path("id") id: String, @Body plan: Plan): Plan

    @DELETE("/plans/{id}")
    suspend fun deletePlan(@Path("id") id: String)

    // Book 相关接口
    @GET("/book/pic")
    suspend fun getBookPic(@Query("page") page: Int): String

    @GET("/book/translate")
    suspend fun getBookTranslation(@Query("page") page: Int): String

    @GET("/book/page")
    suspend fun getBookPage(): Int

    @GET("/book/bookname")
    suspend fun getBookName(): String
}
