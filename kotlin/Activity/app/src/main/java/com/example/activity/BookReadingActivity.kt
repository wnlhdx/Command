@Composable
fun BookReadingScreen(viewModel: BookViewModel = viewModel()) {
    val bookPic by viewModel.bookPic.collectAsState(initial = "")
    val bookTranslation by viewModel.bookTranslation.collectAsState(initial = "")
    val currentPage by viewModel.currentPage.collectAsState()

    Column(modifier = Modifier.fillMaxSize().padding(16.dp)) {
        // 显示当前页的英文原文图片
        Image(
            painter = rememberImagePainter(data = bookPic),
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
            style = MaterialTheme.typography.body1
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

            Text("当前页: $currentPage", style = MaterialTheme.typography.body1)

            Button(
                onClick = { viewModel.nextPage() }
            ) {
                Text("下一页")
            }
        }
    }
}
