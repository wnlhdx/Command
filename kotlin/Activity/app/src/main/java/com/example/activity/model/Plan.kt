package com.example.activity.model

import java.util.Date

data class Plan(
    val planName: String,
    val timeStart: String,
    val timeEnd: String,
    val planDetails: String,
    val projectName: String,
    val projectDetails: String,
    val nightTimeStart: String,
    val nightTimeEnd: String,
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
    val updateTime: String,
)
