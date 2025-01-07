package com.example.nextjsapp.model

data class Book(
    val picUrl: String,     // 图片 URL
    val translation: String, // 翻译内容
    val page: Int,          // 页码
    val bookName: String    // 书名
)
