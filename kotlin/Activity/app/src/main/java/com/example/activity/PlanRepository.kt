package com.example.nextjsapp.repository

import com.example.nextjsapp.model.Plan
import com.example.nextjsapp.network.RetrofitClient

class PlanRepository {
    private val api = RetrofitClient.apiService

    suspend fun getAllPlans(): List<Plan> {
        return api.getAllPlans()
    }

    suspend fun createPlan(plan: Plan): Plan {
        return api.createPlan(plan)
    }

    suspend fun updatePlan(id: String, plan: Plan): Plan {
        return api.updatePlan(id, plan)
    }

    suspend fun deletePlan(id: String) {
        api.deletePlan(id)
    }
}
