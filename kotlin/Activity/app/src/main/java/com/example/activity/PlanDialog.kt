package  com.example.activity;

import androidx.compose.foundation.layout.Column
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import com.example.activity.model.Plan
import java.util.Date

@Composable
fun PlanDialog(plan: Plan?, onDismiss: () -> Unit, onSave: (Plan) -> Unit) {
    val planName = remember { mutableStateOf(plan?.planName ?: "") }
    val planDetails = remember { mutableStateOf(plan?.planDetails ?: "") }
    val projectName = remember { mutableStateOf(plan?.projectName ?: "") }
    val projectDetails = remember { mutableStateOf(plan?.projectDetails ?: "") }
    val projectFinishPercent = remember { mutableStateOf(plan?.projectFinishPercent ?: "") }
    val typeOfLearn = remember { mutableStateOf(plan?.typeOfLearn ?: "") }
    val bookName = remember { mutableStateOf(plan?.bookName ?: "") }
    val bookContent = remember { mutableStateOf(plan?.bookContent ?: "") }
    val majorIn = remember { mutableStateOf(plan?.majorIn ?: "") }
    val projectMonth = remember { mutableStateOf(plan?.projectMonth ?: "") }
    val projectYear = remember { mutableStateOf(plan?.projectYear ?: "") }
    val relaxItem = remember { mutableStateOf(plan?.relaxItem ?: "") }
    val typeDetail = remember { mutableStateOf(plan?.typeDetail ?: "") }

    AlertDialog(
        onDismissRequest = { onDismiss() },
        title = { Text(text = if (plan == null) "新增计划" else "编辑计划") },
        text = {
            Column {
                // 各字段输入框
                TextField(
                    value = planName.value,
                    onValueChange = { planName.value = it },
                    label = { Text("计划名") }
                )
                TextField(
                    value = planDetails.value,
                    onValueChange = { planDetails.value = it },
                    label = { Text("计划详情") }
                )
                TextField(
                    value = projectName.value,
                    onValueChange = { projectName.value = it },
                    label = { Text("项目名") }
                )
                TextField(
                    value = projectDetails.value,
                    onValueChange = { projectDetails.value = it },
                    label = { Text("项目详情") }
                )
                TextField(
                    value = projectFinishPercent.value,
                    onValueChange = { projectFinishPercent.value = it },
                    label = { Text("完成百分比") }
                )
                TextField(
                    value = typeOfLearn.value,
                    onValueChange = { typeOfLearn.value = it },
                    label = { Text("学习方式") }
                )
                TextField(
                    value = bookName.value,
                    onValueChange = { bookName.value = it },
                    label = { Text("书籍") }
                )
                TextField(
                    value = bookContent.value,
                    onValueChange = { bookContent.value = it },
                    label = { Text("书籍内容") }
                )
                TextField(
                    value = majorIn.value,
                    onValueChange = { majorIn.value = it },
                    label = { Text("学科") }
                )
                TextField(
                    value = projectMonth.value,
                    onValueChange = { projectMonth.value = it },
                    label = { Text("项目月份") }
                )
                TextField(
                    value = projectYear.value,
                    onValueChange = { projectYear.value = it },
                    label = { Text("项目年份") }
                )
                TextField(
                    value = relaxItem.value,
                    onValueChange = { relaxItem.value = it },
                    label = { Text("休闲项目") }
                )
                TextField(
                    value = typeDetail.value,
                    onValueChange = { typeDetail.value = it },
                    label = { Text("学习类型详情") }
                )
                // 更多字段可以继续添加
            }
        },
        confirmButton = {
            Button(onClick = {
                onSave(
                    Plan(
                        planName = planName.value,
                        planDetails = planDetails.value,
                        projectName = projectName.value,
                        projectDetails = projectDetails.value,
                        // 以下字段可添加默认值或根据实际需求调整
                        timeStart = plan?.timeStart ?: "",
                        timeEnd = plan?.timeEnd ?: "",
                        nightTimeStart = plan?.nightTimeStart ?: "",
                        nightTimeEnd = plan?.nightTimeEnd ?: "",
                        dayOfWeek = plan?.dayOfWeek ?: "",
                        bookName = plan?.bookName ?: "",
                        bookContent = plan?.bookContent ?: "",
                        projectFinishPercent = plan?.projectFinishPercent ?: "0%",
                        majorIn = plan?.majorIn ?: "",
                        projectMonth = plan?.projectMonth ?: "",
                        projectYear = plan?.projectYear ?: "",
                        relaxItem = plan?.relaxItem ?: "",
                        relaxItemForeign = plan?.relaxItemForeign ?: "",
                        typeOfLearn = plan?.typeOfLearn ?: "",
                        typeDetail = plan?.typeDetail ?: "",
                        standardLearn = plan?.standardLearn ?: "",
                        updateTime = plan?.updateTime ?: "",
                    )
                )
            }) {
                Text("保存")
            }
        },
        dismissButton = {
            Button(onClick = { onDismiss() }) {
                Text("取消")
            }
        }
    )
}
