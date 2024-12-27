import retrofit2.http.Body
import retrofit2.http.DELETE
import retrofit2.http.GET
import retrofit2.http.POST
import retrofit2.http.PUT
import retrofit2.Call
import retrofit2.http.Path


// 数据模型
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
    val bookPage: String
)

// Retrofit接口
interface PlanApi {
    @GET("plans/all")
    fun getPlans(): Call<List<Plan>>

    @POST("plans/add")
    fun addPlan(@Body plan: Plan): Call<Plan>

    @PUT("plans/update")
    fun updatePlan(@Body plan: Plan): Call<Plan>

    @DELETE("plans/delete/{planName}")
    fun deletePlan(@Path("planName") planName: String): Call<Void>
}
