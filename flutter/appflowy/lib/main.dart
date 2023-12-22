import 'package:appflowy/todoitem.dart';
import 'package:flutter/material.dart';  

  
void main() => runApp(MyApp());  
  
class MyApp extends StatelessWidget {  
  @override  
  Widget build(BuildContext context) {  
    return MaterialApp(  
      title: 'My App',  
      theme: ThemeData(primarySwatch: Colors.blue),  
      home: MyHomePage(),  
    );  
  }  
}  
  
class MyHomePage extends StatefulWidget {  
  @override  
  _MyHomePageState createState() => _MyHomePageState();  
}  
  
class _MyHomePageState extends State<MyHomePage> {  
  List<TodoItem> todoItems = [];  
  
  void addTodoItem() {  
  final TextEditingController controller = TextEditingController();  
  final FocusNode focusNode = FocusNode();  
   TextField textField = TextField(controller: controller, focusNode: focusNode);  
  textField.decoration = InputDecoration(hintText: 'Add a new todo item');  
  textField.onChanged = (value) {  
    // 处理输入内容并添加到todoItems列表中  
    String? itemText = value; // 添加了问号，允许itemText为null  
    // TODO: 在这里添加逻辑来处理输入内容，例如验证、存储等  
    // 示例：将输入的文本内容添加到todoItems列表中  
    TodoItem todoItem = TodoItem(itemText); // 创建一个TodoItem对象，并使用输入的文本内容作为参数  
    todoItems.add(todoItem); // 将TodoItem对象添加到todoItems列表中  
    // TODO: 根据需要更新UI，例如重新构建ListView或显示已完成的待办事项等  
  };  
  focusNode.requestFocus(); // 请求焦点，以便用户可以开始输入文本  
  // TODO: 根据需要将文本字段组件添加到UI中，例如放在一个容器中，并在需要时显示该容器。  
}
  
  @override  
  Widget build(BuildContext context) {  
    return Scaffold(  
      appBar: AppBar(title: Text('My App')),  
      body: Container(  
        child: ListView.builder(  
          itemCount: todoItems.length, // 待办事项列表的长度等于todoItems列表的长度  
          itemBuilder: (context, index) {  
            return ListTile(title: Text(todoItems[index].itemText)); // 显示每个待办事项的文本内容  
          },  
        ),  
      ),  
      floatingActionButton: FloatingActionButton(  
        onPressed: () {  
          addTodoItem(); // 调用addTodoItem方法来添加新的待办事项或笔记  
        },  
        child: Icon(Icons.add), // 使用添加图标作为按钮的图标  
      ),  
    );  
  }  
}