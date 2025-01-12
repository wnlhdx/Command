package  com.example.activity;

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.example.activity.viewmodel.BookViewModel
import androidx.lifecycle.viewmodel.compose.viewModel
import coil.compose.rememberAsyncImagePainter


@Composable
fun BookReadingScreen(viewModel: BookViewModel = viewModel()) {
    val bookPic by viewModel.bookPic.collectAsState(initial = "")
    val bookTranslation by viewModel.bookTranslation.collectAsState(initial = "")
    val currentPage by viewModel.currentPage.collectAsState()

    Column(modifier = Modifier.fillMaxSize().padding(16.dp)) {
        // 显示当前页的英文原文图片
        Image(
            painter = rememberAsyncImagePainter(model = bookPic),
            contentDescription = "书籍图片",
            modifier = Modifier
                .fillMaxWidth()
                .height(300.dp)
        )

        Spacer(modifier = Modifier.height(16.dp))

        // 显示翻译的文字
        Text(
            text = bookTranslation,
            modifier = Modifier.fillMaxWidth(),
            style = MaterialTheme.typography.bodyLarge
        )

        Spacer(modifier = Modifier.height(16.dp))

        // 上下页按钮
        Row(
            horizontalArrangement = Arrangement.SpaceBetween,
            modifier = Modifier.fillMaxWidth()
        ) {
            Button(
                onClick = { viewModel.previousPage() },
                enabled = currentPage > 1
            ) {
                Text("上一页")
            }

            Text("当前页: $currentPage", style = MaterialTheme.typography.bodyLarge)

            Button(
                onClick = { viewModel.nextPage() }
            ) {
                Text("下一页")
            }
        }
    }
}
