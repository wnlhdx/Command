*** Test Cases ***
Login Test
    [Documentation]  This is a test case for login functionality
    Open Browser  https://www.example.com  chrome
    Input Text  username_field  testuser
    Input Password  password_field  12345
    Click Button  login_button
    Page Should Contain  Welcome, testuser