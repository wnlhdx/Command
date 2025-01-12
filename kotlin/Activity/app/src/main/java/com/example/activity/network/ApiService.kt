package com.example.activity.network

import com.example.activity.model.Plan
import retrofit2.http.*

interface ApiService {

    // Plan 相关接口
    @GET("/plans/all")
    suspend fun getAllPlans(): List<Plan>

    @POST("/plans/add")
    suspend fun createPlan(@Body plan: Plan): Plan

    @PUT("/plans/{planName}")
    suspend fun updatePlan(@Path("planName") id: String, @Body plan: Plan): Plan

    @DELETE("/del/{planName}")
    suspend fun deletePlan(@Path("planName") id: String)

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
