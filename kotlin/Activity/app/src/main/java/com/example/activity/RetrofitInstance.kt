import okhttp3.OkHttpClient
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response

object RetrofitInstance {
    private const val BASE_URL = "http://192.168.1.3:8080/"

    private val retrofit: Retrofit by lazy {
        Retrofit.Builder()
            .baseUrl(BASE_URL)
            .addConverterFactory(GsonConverterFactory.create())
            .client(OkHttpClient.Builder().build())
            .build()
    }

    val planApi: PlanApi by lazy {
        retrofit.create(PlanApi::class.java)
    }
}

fun fetchPlans(callback: (List<Plan>) -> Unit) {
    RetrofitInstance.planApi.getPlans().enqueue(object : Callback<List<Plan>> {
        override fun onResponse(call: Call<List<Plan>>, response: Response<List<Plan>>) {
            if (response.isSuccessful) {
                callback(response.body() ?: emptyList())
            }
        }

        override fun onFailure(call: Call<List<Plan>>, t: Throwable) {
            // Handle failure
        }
    })
}

fun savePlan(plan: Plan, isEditing: Boolean, callback: () -> Unit) {
    val call = if (isEditing) {
        RetrofitInstance.planApi.updatePlan(plan)
    } else {
        RetrofitInstance.planApi.addPlan(plan)
    }

    call.enqueue(object : Callback<Plan> {
        override fun onResponse(call: Call<Plan>, response: Response<Plan>) {
            if (response.isSuccessful) {
                callback()
            }
        }

        override fun onFailure(call: Call<Plan>, t: Throwable) {
            // Handle failure
        }
    })
}

fun deletePlan(planName: String, callback: () -> Unit) {
    RetrofitInstance.planApi.deletePlan(planName).enqueue(object : Callback<Void> {
        override fun onResponse(call: Call<Void>, response: Response<Void>) {
            if (response.isSuccessful) {
                callback()
            }
        }

        override fun onFailure(call: Call<Void>, t: Throwable) {
            // Handle failure
        }
    })
}
