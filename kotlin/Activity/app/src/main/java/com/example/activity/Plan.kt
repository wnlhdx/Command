package com.example.nextjsapp.model

import java.util.Date

data class Plan(
    val planName: String,
    val timeStart: Date,
    val timeEnd: Date,
    val planDetails: String,
    val projectName: String,
    val projectDetails: String,
    val nightTimeStart: Date,
    val nightTimeEnd: Date,
    val projectFinishPercent: String,
    val dayOfWeek: String,
    val bookName: String,
    val bookContent: String,
    val majorIn: String,
    val projectMonth: String,
    val projectYear: String,
    val relaxItem: String,
    val relaxItemForeign: String,
    val typeOfLearn: String,
    val typeDetail: String,
    val standardLearn: String,
    val updateTime: Date,
    val bookPage: String
)
