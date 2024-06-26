#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

#define MAXUSERS 100
#define USERNAMELENGTH 50
#define PASSWORDLENGTH 50
#define UIDLENGTH 10
#define EMPLOYEE_FILE "Employees.bin"
#define FILENAME "Login.bin"

#define RESET 7
#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define BLUE (FOREGROUND_BLUE)
#define CYAN (FOREGROUND_GREEN | FOREGROUND_BLUE)

// XOR key for encryption and decryption
#define XOR_KEY 0xAA

// Define the User structure
struct User {
    int uid;
    char username[USERNAMELENGTH];
    char password[PASSWORDLENGTH];
    char status; // 'P' for Pending, 'A' for Approved
};

struct Payroll {
    double salary;
    double allowances;
    double overtime;
    double bonuses;
    double tax_deduction;
    double pf_deduction;
    double insurance_premium;
    double net_salary;
    int year;
};

struct Employee {
    char username[USERNAMELENGTH];
    char password[PASSWORDLENGTH];
    int uid;
    struct Payroll payrolls[100]; // Support for multiple years
    int currentYear; // Tracks the current year of payroll processing
    char status; // 'A' for Active, 'I' for Inactive, 'T' for Termination
    char position[USERNAMELENGTH];
    unsigned long long int contact_no;
    char email[100];
    char gender[2];
    char marital_status[2]; // 'M' for Married, 'S' for Single
};

struct User userList[MAXUSERS];
int currentUserCount = 0;
int latestUID = 1;  // Starting point for UIDs

struct Employee employeeList[MAXUSERS];
int currentEmployeeCount = 0;


//Function Declaration
void setColor(int color);
void clearScreen();
void displayLoading();
void displayExiting();
void getPasswordInput(char *password);
void saveUserData();
void loadUserData();
void saveEmployeeData();
void loadEmployeeData();
void createUID(int* uid);
bool continuePrompt();
void registerUser();
void updatePersonalDetails();
void updatePassword();
void resetUserPassword();
void clearInputBuffer();
void viewPaySlip(int uid);
void clientMenu(int uid);
int authenticateUser();
void checkApprovalNotice(int uid);
void employeeStatusNotice();
void mainClientMenu();
void saveAdminPassword(const char* password);
int loadAdminPassword(char* password, int maxLength);
bool isInitialAdminPasswordSet();
void saveInitialAdminPassword();
int validateAdminPassword(const char* password);
void adminChangeAdminPassword();
int checkUserApprovalStatus(int uid, char* password);
void adminApproveNewEmployee();
void adminEditEmployeeDetails();
void adminDeleteEmployeeRecord();
void adminSearchEmployeeRecord();
void adminChangeEmployeeStatus();
void adminViewAllEmployeeDetails();
void adminPayrollProcessing();
void updateNewPayrollProcessing();
void modifyPayrollEntry();
void viewAllEmployeePayrollSummary();
void searchEmployeePayrollSummary();
void displayPayrollGuideline();
void viewPersonalInformation(int uid);
void encryptionMenu();
void decryptionMenu();
void encryptionDecryptionTool();
void viewAllEmployeeAuditLog();
void searchEmployeeAuditLog();
void adminEmployeeInfoManagement();
void adminMenu();
void mainMenu();

//Main Function
int main() {
    loadUserData();
    loadEmployeeData();

    // Setup initial admin password if not already set
    if (!isInitialAdminPasswordSet()) {
        saveInitialAdminPassword();
    }

    mainMenu();
    
    return 0;
}

//Function Definition
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clearScreen() {
    system("cls");
}

void displayLoading() {
    setColor(YELLOW);
    printf("Loading");
    for (int i = 0; i < 3; i++) {
        setColor(YELLOW);
        printf(".");
        fflush(stdout);
        Sleep(500);
    }
    printf("\n");
    clearScreen();
}

void displayExiting() {
    clearScreen();
    setColor(RED);
    printf("Exiting");
    for (int i = 0; i < 3; i++) {
        setColor(RED);
        printf(".");
        fflush(stdout);
        Sleep(500);
    }
    printf("\n");
    setColor(RESET);
    exit(0);
}

void getPasswordInput(char *password) {
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();
        if (ch == '\r') {
            break;
        } else if (ch == '\b' && i > 0) {
            i--;
            printf("\b \b");
        } else if (ch != '\b') {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
}

void saveUserData() {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        setColor(RED);
        printf("Error: Cannot open file for writing!\n");
        setColor(RESET);
        return;
    }

    fwrite(&currentUserCount, sizeof(int), 1, file);
    fwrite(&latestUID, sizeof(int), 1, file);  // Save latest UID
    fwrite(userList, sizeof(struct User), currentUserCount, file);
    fclose(file);
}

void loadUserData() {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        setColor(RED);
        printf("Error: Cannot open file for reading!\n");
        setColor(RESET);
        return;
    }

    fread(&currentUserCount, sizeof(int), 1, file);
    fread(&latestUID, sizeof(int), 1, file);  // Load latest UID
    fread(userList, sizeof(struct User), currentUserCount, file);
    fclose(file);
}

void saveEmployeeData() {
    FILE *file = fopen(EMPLOYEE_FILE, "wb");
    if (file == NULL) {
        setColor(RED);
        printf("Error: Cannot open employee file for writing!\n");
        setColor(RESET);
        return;
    }

    fwrite(&currentEmployeeCount, sizeof(int), 1, file);
    fwrite(employeeList, sizeof(struct Employee), currentEmployeeCount, file);
    fclose(file);
}

void loadEmployeeData() {
    FILE *file = fopen(EMPLOYEE_FILE, "rb");
    if (file == NULL) {
        setColor(RED);
        printf("Error: Cannot open employee file for reading!\n");
        setColor(RESET);
        return;
    }

    fread(&currentEmployeeCount, sizeof(int), 1, file);
    fread(employeeList, sizeof(struct Employee), currentEmployeeCount, file);
    fclose(file);
}

void createUID(int* uid) {
    *uid = latestUID++;
}

bool continuePrompt() {
    char check;
    setColor(CYAN);
    printf("Do you want to continue (Y/N): ");
    setColor(RESET);
    scanf(" %c", &check);
    while (check != 'Y' && check != 'y' && check != 'N' && check != 'n') {
        setColor(RED);
        printf("Error: Invalid input! Please enter Y or N: ");
        setColor(RESET);
        scanf(" %c", &check);
    }
    if (check == 'N' || check == 'n') {
        displayExiting();
        exit(0);
    }
    return (check == 'Y' || check == 'y');
}

void registerUser() {
    setColor(BLUE);
    printf("\nRegister New User\n=========================\n");
    setColor(RESET);

    struct User newUser;
    newUser.status = 'P'; // Pending approval

    createUID(&newUser.uid);  // Automatically generate a sequential UID

    setColor(CYAN);
    printf("Generated UID: %d\n", newUser.uid);
    setColor(CYAN);
    printf("Enter Username: ");
    setColor(RESET);
    fflush(stdin);
    fgets(newUser.username, USERNAMELENGTH, stdin);
    newUser.username[strcspn(newUser.username, "\n")] = '\0';

    setColor(CYAN);
    printf("Enter Password: ");
    setColor(RESET);
    getPasswordInput(newUser.password);
    printf("\n");

    userList[currentUserCount++] = newUser;
    saveUserData();

    setColor(GREEN);
    printf("Registration successful! Please wait for admin approval.\n");
    setColor(RESET);
}

void updatePersonalDetails() {
    int uid;
    struct Employee *employee = NULL;

    // Prompt for UID
    setColor(CYAN);
    printf("Enter your UID: ");
    setColor(RESET);
    scanf("%d", &uid);

    // Find the employee by UID
    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            employee = &employeeList[i];
            break;
        }
    }

    if (employee == NULL) {
        setColor(RED);
        printf("Error: Invalid UID!\n");
        setColor(RESET);
        return;
    }

    // Prompt for and read contact number
    setColor(CYAN);
    printf("Enter contact number (10 digits only): ");
    setColor(RESET);
    scanf("%llu", &employee->contact_no);

    // Validate contact number length
    unsigned long long int temp = employee->contact_no;
    int digitCount = 0;
    while (temp != 0) {
        temp /= 10;
        digitCount++;
    }

    if (digitCount != 10) {
        setColor(RED);
        printf("Error: Contact number must have exactly 10 digits!\n");
        setColor(RESET);
        return;
    }

    // Prompt for and read email address
    setColor(CYAN);
    printf("Enter email address: ");
    setColor(RESET);
    scanf("%99s", employee->email);

    // Validate email format
    if (strlen(employee->email) < 10 || strcmp(employee->email + strlen(employee->email) - 10, "@gmail.com") != 0) {
        setColor(RED);
        printf("Error: Invalid email format!\n");
        setColor(RESET);
        return;
    }

    // Prompt for and read gender
    setColor(CYAN);
    printf("Enter gender (M/F/O): ");
    setColor(RESET);
    scanf("%1s", employee->gender);

    if (strcmp(employee->gender, "M") != 0 && strcmp(employee->gender, "F") != 0 && strcmp(employee->gender, "O") != 0) {
        setColor(RED);
        printf("Error: Invalid gender input!\n");
        setColor(RESET);
        return;
    }

    // Prompt for and read marital status
    setColor(CYAN);
    printf("Enter marital status (M/S): ");
    setColor(RESET);
    scanf("%1s", employee->marital_status);

    if (strcmp(employee->marital_status, "M") != 0 && strcmp(employee->marital_status, "S") != 0) {
        setColor(RED);
        printf("Error: Invalid marital status input!\n");
        setColor(RESET);
        return;
    }

    saveEmployeeData();
    setColor(GREEN);
    printf("Personal details updated successfully!\n");
    setColor(RESET);
}


void updatePassword() {
    int uid;
    char oldPassword[PASSWORDLENGTH], newPassword[PASSWORDLENGTH], reenterPassword[PASSWORDLENGTH];
    setColor(CYAN);
    printf("Enter your UID: ");
    setColor(RESET);
    scanf("%d", &uid);

    setColor(CYAN);
    printf("Enter your old password: ");
    setColor(RESET);
    getPasswordInput(oldPassword);
    printf("\n");

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid && strcmp(employeeList[i].password, oldPassword) == 0) {
            if (employeeList[i].status != 'A') {
                setColor(RED);
                printf("Error: Your account is not active!\n");
                setColor(RESET);
                return;
            }

            setColor(CYAN);
            printf("Enter a new password: ");
            setColor(RESET);
            getPasswordInput(newPassword);
            printf("\n");

            setColor(CYAN);
            printf("Re-enter your new password: ");
            setColor(RESET);
            getPasswordInput(reenterPassword);
            printf("\n");

            if (strcmp(newPassword, reenterPassword) != 0) {
                setColor(RED);
                printf("Error: Passwords do not match!\n");
                setColor(RESET);
                return;
            }

            strcpy(employeeList[i].password, newPassword);
            saveEmployeeData();

            setColor(GREEN);
            printf("Password changed successfully!\nYour Password is %s\nCaution: Keep your Password noted down and safe!\n", newPassword);
            setColor(RESET);
            return;
        }
    }

    setColor(RED);
    printf("Error: Invalid UID or old password!\n");
    setColor(RESET);
}

void resetUserPassword() {
    int uid;
    char newPassword[PASSWORDLENGTH], reenterPassword[PASSWORDLENGTH];
    setColor(CYAN);
    printf("Enter your UID: ");
    setColor(RESET);
    scanf("%d", &uid);

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            setColor(CYAN);
            printf("Enter a new password: ");
            setColor(RESET);
            getPasswordInput(newPassword);
            printf("\n");

            setColor(CYAN);
            printf("Re-enter your new password: ");
            setColor(RESET);
            getPasswordInput(reenterPassword);
            printf("\n");

            if (strcmp(newPassword, reenterPassword) != 0) {
                setColor(RED);
                printf("Error: Passwords do not match!\n");
                setColor(RESET);
                return;
            }

            strcpy(employeeList[i].password, newPassword);
            saveEmployeeData();

            setColor(GREEN);
            printf("Password reset successfully!\nYour Password is %s\nCaution: Keep your Password noted down and safe!\n", newPassword);
            setColor(RESET);
            return;
        }
    }

    setColor(RED);
    printf("Error: Invalid UID!\n");
    setColor(RESET);
}

void clearInputBuffer() {
    while (getchar() != '\n');
}

void viewPersonalInformation(int uid) {
    struct Employee *employee = NULL;

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            employee = &employeeList[i];
            break;
        }
    }

    if (employee == NULL) {
        setColor(RED);
        printf("Error: Invalid UID!\n");
        setColor(RESET);
        return;
    }

    setColor(GREEN);
    printf("\nPersonal Information\n=========================\n");
    printf("UID: %d\n", employee->uid);
    printf("Username: %s\n", employee->username);
    printf("Gender: %s\n", employee->gender);
    printf("Marital Status: %s\n", employee->marital_status);
    printf("Position: %s\n", employee->position);
    printf("Contact No: %llu\n", employee->contact_no);
    printf("Email: %s\n", employee->email);
    setColor(RESET);
}

void viewPaySlip(int uid) {
    struct Employee *employee = NULL;
    int year;

    // Find the employee by UID
    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            employee = &employeeList[i];
            break;
        }
    }

    if (employee == NULL) {
        setColor(RED);
        printf("Error: Invalid UID!\n");
        setColor(RESET);
        return;
    }

    printf("Enter the year you want to view (1 to %d): ", employee->currentYear);
    scanf("%d", &year);

    if (year < 1 || year > employee->currentYear) {
        setColor(RED);
        printf("Error: Invalid year!\n");
        setColor(RESET);
        return;
    }

    struct Payroll *payroll = &employee->payrolls[year];
    double annual_salary = payroll->net_salary * 12;

    setColor(GREEN);
    printf("\nPay Slip for %s (Year %d)\n=========================\n", employee->username, year);
    printf("Basic Salary: %.2f\n", payroll->salary);
    printf("Allowances: %.2f\n", payroll->allowances);
    printf("Overtime: %.2f\n", payroll->overtime);
    printf("Bonuses: %.2f\n", payroll->bonuses);
    printf("Tax Deduction: %.2f\n", payroll->tax_deduction);
    printf("Provident Fund: %.2f\n", payroll->pf_deduction);
    printf("Insurance Premium: %.2f\n", payroll->insurance_premium);
    printf("Net Salary: %.2f\n", payroll->net_salary);
    printf("Annual Salary: %.2f\n", annual_salary);
    setColor(RESET);
}

void viewClientAuditLog(int uid) {
    struct Employee *employee = NULL;

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            employee = &employeeList[i];
            break;
        }
    }

    if (employee == NULL) {
        setColor(RED);
        printf("Error: Invalid UID!\n");
        setColor(RESET);
        return;
    }

    setColor(CYAN);
    printf("\nAudit Log for %s\n=========================\n", employee->username);
    setColor(RESET);

    for (int year = 1; year <= employee->currentYear; year++) {
        struct Payroll *payroll = &employee->payrolls[year];
        double annual_salary = payroll->net_salary * 12;

        setColor(GREEN);
        printf("\nPayroll for Year %d\n-------------------------\n", year);
        printf("Basic Salary: %.2f\n", payroll->salary);
        printf("Allowances: %.2f\n", payroll->allowances);
        printf("Overtime: %.2f\n", payroll->overtime);
        printf("Bonuses: %.2f\n", payroll->bonuses);
        printf("Tax Deduction: %.2f\n", payroll->tax_deduction);
        printf("Provident Fund: %.2f\n", payroll->pf_deduction);
        printf("Insurance Premium: %.2f\n", payroll->insurance_premium);
        printf("Net Salary: %.2f\n", payroll->net_salary);
        printf("Annual Salary: %.2f\n", annual_salary);
        setColor(RESET);
    }
}

void clientMenu(int uid) {
    int option;
    char check;
    bool maritalStatusSet = false;

    // Update marital status if necessary
    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid && employeeList[i].marital_status[0] != '\0') {
            maritalStatusSet = true;
            break;
        }
    }

    while (1) {
        clearScreen();
        displayLoading();
        setColor(BLUE);
        printf("\nCipherPayroll - Employee Payroll System - Client Menu\n=========================\n");
        setColor(RESET);

        // Show UID and username after successful login
        struct Employee *loggedInEmployee = NULL;
        for (int i = 0; i < currentEmployeeCount; i++) {
            if (employeeList[i].uid == uid) {
                loggedInEmployee = &employeeList[i];
                break;
            }
        }

        if (loggedInEmployee != NULL) {
            setColor(GREEN);
            printf("Logged in as UID: %d, Username: %s\n", loggedInEmployee->uid, loggedInEmployee->username);
            setColor(RESET);
        }

        if (!maritalStatusSet) {
            setColor(RED);
            printf("Notice: Please update your marital status in Personal Information.\n");
            setColor(RESET);
        }

        setColor(CYAN);
        printf("1. Personal Details\n");
        printf("2. Payroll Information\n");
        printf("3. View Audit Log\n");
        printf("4. Go back to main client menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                displayLoading();
                setColor(BLUE);
                printf("\nEmployee Payroll System - Personal Details\n=========================\n");
                setColor(RESET);
                int subOption;
                setColor(CYAN);
                printf("1) View Personal Information\n");
                printf("2) Update Personal Information\n");
                setColor(GREEN);
                printf("Enter your choice: ");
                scanf("%d", &subOption);
                switch (subOption) {
                    case 1:
                        viewPersonalInformation(uid);
                        break;
                    case 2:
                        updatePersonalDetails();
                        maritalStatusSet = true; // Update the status after changing
                        break;
                    default:
                        setColor(RED);
                        printf("Error: Invalid option!\n");
                        setColor(RESET);
                        break;
                }
                break;
            case 2:
                displayLoading();
                setColor(BLUE);
                printf("\nEmployee Payroll System - Payroll Information\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("1) View Pay Slip\n");
                printf("2) View Payroll Guideline\n");
                setColor(GREEN);
                printf("Enter your choice: ");
                scanf("%d", &subOption);
                switch (subOption) {
                    case 1:
                        viewPaySlip(uid);
                        break;
                    case 2:
                        displayPayrollGuideline();
                        break;
                    default:
                        setColor(RED);
                        printf("Error: Invalid option!\n");
                        setColor(RESET);
                        break;
                }
                break;
            case 3:
                viewClientAuditLog(uid);
                break;
            case 4:
                return; // Go back to the main client menu
            default:
                displayLoading();
                printf("\nEmployee Payroll System - Client Menu\n=========================\n");
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}

int authenticateUser() {
    int uid;
    char password[PASSWORDLENGTH];
    setColor(CYAN);
    printf("Enter your UID: ");
    setColor(RESET);
    scanf("%d", &uid);

    setColor(CYAN);
    printf("Enter your password: ");
    setColor(RESET);
    getPasswordInput(password);  // Get password input securely
    printf("\n");

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid && strcmp(employeeList[i].password, password) == 0) {
            if (employeeList[i].status == 'A') {
                setColor(GREEN);
                printf("Login successful!\n");
                printf("Welcome, %s!\n", employeeList[i].username);
                setColor(RESET);
                clientMenu(uid);  // Go to client menu after successful login
                return 1;
            } else {
                setColor(RED);
                printf("Error: Your account is not active!\n");
                setColor(RESET);
                return 0;
            }
        }
    }

    setColor(RED);
    printf("Error: Invalid UID or password!\n");
    setColor(RESET);
    return 0;
}

void checkApprovalNotice(int uid) {
    char password[PASSWORDLENGTH];
    setColor(CYAN);
    printf("Enter your UID: ");
    setColor(RESET);
    scanf("%d", &uid);

    setColor(CYAN);
    printf("Enter your password: ");
    setColor(RESET);
    getPasswordInput(password);  // Get password input securely
    printf("\n");

    int status = checkUserApprovalStatus(uid, password);
    if (status == 1) {
        setColor(GREEN);
        printf("Your registration is approved.\n");
    } else if (status == 0) {
        setColor(YELLOW);
        printf("Your registration is pending approval.\n");
    } else {
        setColor(RED);
        printf("Error: Invalid UID or password!\n");
    }
    setColor(RESET);
}

void employeeStatusNotice() {
    int uid;
    char password[PASSWORDLENGTH];

    setColor(CYAN);
    printf("Enter your UID: ");
    setColor(RESET);
    scanf("%d", &uid);

    setColor(CYAN);
    printf("Enter your password: ");
    setColor(RESET);
    getPasswordInput(password);  // Get password input securely
    printf("\n");

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid && strcmp(employeeList[i].password, password) == 0) {
            if (employeeList[i].status == 'A') {
                setColor(GREEN);
                printf("Status: %c\n", employeeList[i].status);
            } else if (employeeList[i].status == 'I') {
                setColor(YELLOW);
                printf("Status: %c\n", employeeList[i].status);
            } else if (employeeList[i].status == 'T') {
                setColor(RED);
                printf("Status: %c\n", employeeList[i].status);
            }
            printf("Position: %s\n", employeeList[i].position);
            setColor(RESET);
            return;
        }
    }

    setColor(RED);
    printf("Error: Invalid UID or password!\n");
    setColor(RESET);
}

void mainClientMenu() {
    int option;
    int isLoggedIn = 0;
    char check = '\0';
    int uid = 0;
    while (1) {
        clearScreen();
        displayLoading();
        setColor(BLUE);
        printf("\nCipherPayroll - Main Client Menu\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. Register\n");
        printf("2. Log In\n");
        printf("3. Change Password\n");
        printf("4. Forgot Password?\n");
        printf("5. Check Approval Notice\n");
        printf("6. Employee Status Notice\n");
        printf("7. Go back to main menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                displayLoading();
                setColor(BLUE);
                printf("\nCipherPayroll\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("1. Register\n");
                registerUser();
                break;
            case 2:
                displayLoading();
                setColor(BLUE);
                printf("\nCipherPayroll\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("2. Log In\n");
                isLoggedIn = authenticateUser();
                if (isLoggedIn) {
                    break;
                }
                break;
            case 3:
                displayLoading();
                setColor(BLUE);
                printf("\nCipherPayroll\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("3. Change Password\n");
                updatePassword();
                break;
            case 4:
                displayLoading();
                setColor(BLUE);
                printf("\nCipherPayroll\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("4. Forgot Password?\n");
                resetUserPassword();
                break;
            case 5:
                displayLoading();
                setColor(BLUE);
                printf("\nCipherPayroll\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("5. Check Approval Notice\n");
                checkApprovalNotice(uid);
                break;
            case 6:
                displayLoading();
                setColor(BLUE);
                printf("\nCipherPayroll\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("6. Employee Status and Position Notice\n");
                employeeStatusNotice();
                break;
            case 7:
                return;
            default:
                displayLoading();
                printf("\nCipherPayroll\n=========================\n");
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}

void saveAdminPassword(const char* password) {
    FILE* file = fopen("AdminPassword.txt", "w");
    if (file == NULL) {
        setColor(RED);
        printf("Error: Cannot open AdminPassword.txt for writing!\n");
        setColor(RESET);
        return;
    }
    fprintf(file, "%s\n", password);
    fclose(file);
}

int loadAdminPassword(char* password, int maxLength) {
    FILE* file = fopen("AdminPassword.txt", "r");
    if (file == NULL) {
        setColor(RED);
        printf("Error: Cannot open AdminPassword.txt for reading!\n");
        setColor(RESET);
        return 0;
    }
    if (fgets(password, maxLength, file) == NULL) {
        fclose(file);
        return 0; // File is empty or read error
    }
    // Remove newline character if present
    password[strcspn(password, "\n")] = '\0';
    fclose(file);
    return 1;
}

bool isInitialAdminPasswordSet() {
    FILE *file = fopen("AdminPassword.txt", "r");
    if (file != NULL) {
        fclose(file); // Close the file if it exists
        return true;
    } else {
        return false;
    }
}

void saveInitialAdminPassword() {
    FILE* file = fopen("AdminPassword.txt", "w");
    if (file == NULL) {
        setColor(RED);
        printf("Error: Cannot open AdminPassword.txt for writing!\n");
        setColor(RESET);
        return;
    }
    fprintf(file, "admin123\n"); // Write the initial password
    fclose(file);
}

// Function to validate admin password
int validateAdminPassword(const char* password) {
    char adminPassword[PASSWORDLENGTH];
    if (loadAdminPassword(adminPassword, PASSWORDLENGTH)) {
        return strcmp(password, adminPassword) == 0;
    }
    return 0; // Return false if admin password loading failed
}

void adminChangeAdminPassword() {
    char currentPassword[PASSWORDLENGTH];
    char newPassword[PASSWORDLENGTH];
    char reenterPassword[PASSWORDLENGTH];
    char currentPasswordFromFile[PASSWORDLENGTH]; // Declare currentPasswordFromFile

    clearScreen();
    setColor(BLUE);
    printf("\nChange Admin's Password\n=========================\n");
    setColor(RESET);

    // Check if the admin password is already set
    if (!loadAdminPassword(currentPasswordFromFile, PASSWORDLENGTH)) {
        // Initial setup: prompt for new password
        setColor(CYAN);
        printf("Enter a new admin password: ");
        setColor(RESET);
        getPasswordInput(newPassword);
        printf("\n");

        // Prompt to re-enter new password
        setColor(CYAN);
        printf("Re-enter the new password: ");
        setColor(RESET);
        getPasswordInput(reenterPassword);
        printf("\n");

        // Validate new passwords match
        if (strcmp(newPassword, reenterPassword) != 0) {
            setColor(RED);
            printf("Error: Passwords do not match!\n");
            setColor(RESET);
            printf("\nPress any key to continue...\n");
            _getch();
            return;
        }

        // Update admin password file
        saveAdminPassword(newPassword);

        setColor(GREEN);
        printf("Admin password set successfully!\nYour Password is %s\nCaution: Keep your Password noted down and safe!\n", newPassword);
        setColor(RESET);
        printf("\nPress any key to continue...\n");
        _getch();
    } else {
        // Subsequent password change: prompt for current password
        setColor(CYAN);
        printf("Enter current admin password: ");
        setColor(RESET);
        getPasswordInput(currentPassword);
        printf("\n");

        // Validate current admin password
        if (strcmp(currentPassword, "") == 0 || strcmp(currentPassword, currentPasswordFromFile) != 0) {
            setColor(RED);
            printf("Error: Incorrect current password!\n");
            setColor(RESET);
            printf("\nPress any key to continue...\n");
            _getch();
            return;
        }

        // Prompt for new password
        setColor(CYAN);
        printf("Enter a new password: ");
        setColor(RESET);
        getPasswordInput(newPassword);
        printf("\n");

        // Prompt to re-enter new password
        setColor(CYAN);
        printf("Re-enter the new password: ");
        setColor(RESET);
        getPasswordInput(reenterPassword);
        printf("\n");

        // Validate new passwords match
        if (strcmp(newPassword, reenterPassword) != 0) {
            setColor(RED);
            printf("Error: Passwords do not match!\n");
            setColor(RESET);
            printf("\nPress any key to continue...\n");
            _getch();
            return;
        }

        // Update admin password file
        saveAdminPassword(newPassword);

        setColor(GREEN);
        printf("Admin password changed successfully!\nYour Password is %s\nCaution: Keep your Password noted down and safe!\n", newPassword);
        setColor(RESET);
        printf("\nPress any key to continue...\n");
        _getch();
    }
}
// Check user approval status
int checkUserApprovalStatus(int uid, char* password) {
    for (int i = 0; i < currentUserCount; i++) {
        if (userList[i].uid == uid && strcmp(userList[i].password, password) == 0) {
            return userList[i].status == 'A'; // Return 1 if approved, 0 otherwise
        }
    }
    return -1; // User not found
}

void adminApproveNewEmployee() {
    setColor(BLUE);
    printf("\nApproval for New Employee\n=========================\n");
    setColor(RESET);

    for (int i = 0; i < currentUserCount; i++) {
        if (userList[i].status == 'P') {
            printf("UID: %d, Username: %s\n", userList[i].uid, userList[i].username);
        }
    }

    int uid;
    setColor(CYAN);
    printf("Enter UID of the employee to approve: ");
    setColor(RESET);
    scanf("%d", &uid);

    for (int i = 0; i < currentUserCount; i++) {
        if (userList[i].uid == uid && userList[i].status == 'P') {
            userList[i].status = 'A'; // Set status to Approved

            struct Employee newEmployee;
            strcpy(newEmployee.username, userList[i].username);
            strcpy(newEmployee.password, userList[i].password);
            newEmployee.uid = userList[i].uid;
            newEmployee.currentYear = 0; // No payroll data yet
            newEmployee.status = 'A'; // Set status to Active
            strcpy(newEmployee.position, "Not Assigned"); // Set initial position
            newEmployee.contact_no = 0; // Initial contact number
            strcpy(newEmployee.email, ""); // Initial email
            strcpy(newEmployee.gender, "");
            strcpy(newEmployee.marital_status, "");

            employeeList[currentEmployeeCount++] = newEmployee;

            setColor(GREEN);
            printf("Employee approved successfully!\n");
            printf("UID: %d\n", newEmployee.uid);
            printf("Password: %s\n", newEmployee.password);
            setColor(RESET);

            saveUserData();
            saveEmployeeData();
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found or already approved!\n");
    setColor(RESET);
}
void adminEditEmployeeDetails() {
    int uid, option;
    setColor(BLUE);
    printf("\nEdit Employee Details\n=========================\n");
    setColor(RESET);

    setColor(CYAN);
    printf("Enter UID of the employee to edit: ");
    setColor(RESET);
    if (scanf("%d", &uid) != 1) {
        setColor(RED);
        printf("Error: Invalid UID input!\n");
        setColor(RESET);
        return;
    }

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            while (1) {
                setColor(CYAN);
                printf("\nWhat would you like to edit?\n");
                printf("1. Username\n");
                printf("2. Password\n");
                printf("3. Position\n");
                printf("4. Personal Details\n");
                printf("5. Go back\n");
                setColor(GREEN);
                printf("Enter your choice: ");
                setColor(RESET);
                if (scanf("%d", &option) != 1) {
                    setColor(RED);
                    printf("Error: Invalid option input!\n");
                    setColor(RESET);
                    continue;
                }

                switch (option) {
                    case 1:
                        setColor(CYAN);
                        printf("Enter new username: ");
                        setColor(RESET);
                        getchar(); // Consume newline left by scanf
                        fgets(employeeList[i].username, USERNAMELENGTH, stdin);
                        employeeList[i].username[strcspn(employeeList[i].username, "\n")] = '\0';
                        break;
                    case 2:
                        setColor(CYAN);
                        printf("Enter new password: ");
                        setColor(RESET);
                        getPasswordInput(employeeList[i].password);
                        printf("\n");
                        break;
                    case 3:
                        setColor(CYAN);
                        printf("Enter new position (employee, manager, chairman): ");
                        setColor(RESET);
                        getchar(); // Consume newline left by scanf
                        fgets(employeeList[i].position, USERNAMELENGTH, stdin);
                        employeeList[i].position[strcspn(employeeList[i].position, "\n")] = '\0';
                        if (strcmp(employeeList[i].position, "employee") != 0 && strcmp(employeeList[i].position, "manager") != 0 && strcmp(employeeList[i].position, "chairman") != 0) {
                            setColor(RED);
                            printf("Error: Invalid position!\n");
                            setColor(RESET);
                            return;
                        }

                        // Check if there is already a chairman
                        if (strcmp(employeeList[i].position, "chairman") == 0) {
                            for (int j = 0; j < currentEmployeeCount; j++) {
                                if (j != i && strcmp(employeeList[j].position, "chairman") == 0) {
                                    setColor(RED);
                                    printf("Error: There can only be one chairman in the company!\n");
                                    setColor(RESET);
                                    return;
                                }
                            }
                        }
                        break;
                    case 4:
                        {
                            unsigned long long int newContactNo;
                            char newEmail[USERNAMELENGTH];
                            char newGender[2];

                            setColor(CYAN);
                            printf("Enter new contact number (10 digits only): ");
                            setColor(RESET);
                            if (scanf("%llu", &newContactNo) != 1) {
                                setColor(RED);
                                printf("Error: Invalid contact number input!\n");
                                setColor(RESET);
                                break;
                            }

                            // Validate contact number length
                            unsigned long long int temp = newContactNo;
                            int digitCount = 0;
                            while (temp != 0) {
                                temp /= 10;
                                digitCount++;
                            }

                            if (digitCount != 10) {
                                setColor(RED);
                                printf("Error: Contact number must have exactly 10 digits!\n");
                                setColor(RESET);
                                break;
                            }

                            setColor(CYAN);
                            printf("Enter new email address: ");
                            setColor(RESET);
                            scanf("%s", newEmail);

                            // Validate email format
                            if (strlen(newEmail) < 10 || strcmp(newEmail + strlen(newEmail) - 10, "@gmail.com") != 0) {
                                setColor(RED);
                                printf("Error: Invalid email format!\n");
                                setColor(RESET);
                                break;
                            }

                            setColor(CYAN);
                            printf("Enter gender (M/F/O): ");
                            setColor(RESET);
                            scanf("%1s", newGender);
                            if (strcmp(newGender, "M") != 0 && strcmp(newGender, "F") != 0 && strcmp(newGender, "O") != 0) {
                                setColor(RED);
                                printf("Error: Invalid gender input!\n");
                                setColor(RESET);
                                break;
                            }

                            employeeList[i].contact_no = newContactNo;
                            strcpy(employeeList[i].email, newEmail);
                            strcpy(employeeList[i].gender, newGender);
                        }
                        break;
                    case 5:
                        saveEmployeeData();
                        setColor(GREEN);
                        printf("Employee details updated successfully!\n");
                        setColor(RESET);
                        return;
                    default:
                        setColor(RED);
                        printf("Error: Invalid option!\n");
                        setColor(RESET);
                        break;
                }
            }
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void adminDeleteEmployeeRecord() {
    int uid;
    setColor(BLUE);
    printf("\nDelete Employee Record\n=========================\n");
    setColor(RESET);

    setColor(CYAN);
    printf("Enter UID of the employee to delete: ");
    setColor(RESET);
    scanf("%d", &uid);

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            for (int j = i; j < currentEmployeeCount - 1; j++) {
                employeeList[j] = employeeList[j + 1];
            }
            currentEmployeeCount--;

            saveEmployeeData();

            setColor(GREEN);
            printf("Employee record deleted successfully!\n");
            setColor(RESET);
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void adminSearchEmployeeRecord() {
    int uid;
    setColor(BLUE);
    printf("\nSearch Employee Record\n=========================\n");
    setColor(RESET);

    setColor(CYAN);
    printf("Enter UID of the employee to search: ");
    setColor(RESET);
    scanf("%d", &uid);

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            struct Employee *employee = &employeeList[i];

            setColor(GREEN);
            printf("\nEmployee Details\n=========================\n");
            printf("UID: %d\n", employee->uid);
            printf("Username: %s\n", employee->username);
            printf("Position: %s\n", employee->position);
            printf("Status: %c\n", employee->status);
            printf("Contact No: %llu\n", employee->contact_no);
            printf("Email: %s\n", employee->email);
            printf("Gender: %s\n", employee->gender);
            printf("Marital Status: %s\n", employee->marital_status);
            setColor(RESET);

            for (int year = 1; year <= employee->currentYear; year++) {
                struct Payroll *payroll = &employee->payrolls[year];
                double annual_salary = payroll->net_salary * 12;

                setColor(GREEN);
                printf("\nPayroll for Year %d\n-------------------------\n", year);
                printf("Basic Salary: %.2f\n", payroll->salary);
                printf("Allowances: %.2f\n", payroll->allowances);
                printf("Overtime: %.2f\n", payroll->overtime);
                printf("Bonuses: %.2f\n", payroll->bonuses);
                printf("Tax Deduction: %.2f\n", payroll->tax_deduction);
                printf("Provident Fund: %.2f\n", payroll->pf_deduction);
                printf("Insurance Premium: %.2f\n", payroll->insurance_premium);
                printf("Net Salary: %.2f\n", payroll->net_salary);
                printf("Annual Salary: %.2f\n", annual_salary);
                setColor(RESET);
            }
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void adminChangeEmployeeStatus() {
    int uid;
    char status;
    setColor(BLUE);
    printf("\nChange Employee Status\n=========================\n");
    setColor(RESET);

    setColor(CYAN);
    printf("Enter UID of employee: ");
    setColor(RESET);
    scanf("%d", &uid);

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            setColor(CYAN);
            printf("Enter new status (A for Active, I for Inactive, T for Terminated): ");
            setColor(RESET);
            getchar();
            status = getchar();

            if (status == 'A' || status == 'I') {
                employeeList[i].status = status;
                saveEmployeeData();
                setColor(GREEN);
                printf("Employee status changed successfully!\n");
                setColor(RESET);
            } else if (status == 'T') {
                adminDeleteEmployeeRecord();
            } else {
                setColor(RED);
                printf("Error: Invalid status!\n");
                setColor(RESET);
            }
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void adminViewAllEmployeeDetails() {
    setColor(BLUE);
    printf("\nAll Employee Details\n=========================\n");
    setColor(RESET);

    for (int i = 0; i < currentEmployeeCount; i++) {
        printf("UID: %d, Username: %s, Gender: %s, Position: %s, Status: %c, Contact No: %llu,\n Email: %s\n",
            employeeList[i].uid, employeeList[i].username, employeeList[i].gender, employeeList[i].position,
            employeeList[i].status, employeeList[i].contact_no, employeeList[i].email);
    }
}

void adminPayrollProcessing() {
    int option;

    while (1) {
        displayLoading();
        setColor(BLUE);
        printf("\nPayroll Processing\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. Update New Payroll Processing\n");
        printf("2. Modify Entry of Payroll Processing\n");
        printf("3. View All Employee Payroll Summary\n");
        printf("4. View Payroll Guideline\n");
        printf("5. Search Employee Payroll Summary\n");
        printf("6. Back to Admin Menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                updateNewPayrollProcessing();
                break;
            case 2:
                modifyPayrollEntry();
                break;
            case 3:
                viewAllEmployeePayrollSummary();
                break;
            case 4:
                displayPayrollGuideline();
                break;
            case 5:
                searchEmployeePayrollSummary();
                break;
            case 6:
                return;
            default:
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}
void updateNewPayrollProcessing() {
    int uid;
    double salary, allowances, overtime, bonuses, insurance_premium;

    setColor(CYAN);
    printf("\nUpdate New Payroll Processing\n=========================\n");
    setColor(RESET);
    
    printf("Enter UID of the employee: ");
    scanf("%d", &uid);

    // Check if the employee's marital status is recorded
    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid && strcmp(employeeList[i].marital_status, "") == 0) {
            setColor(RED);
            printf("Error: Employee must update their marital status first.\n");
            setColor(RESET);
            return;
        }
    }

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            setColor(CYAN);
            printf("Enter new monthly salary: ");
            setColor(RESET);
            scanf("%lf", &salary);
            
            setColor(CYAN);
            printf("Enter new allowances: ");
            setColor(RESET);
            scanf("%lf", &allowances);
            
            setColor(CYAN);
            printf("Enter new overtime payment: ");
            setColor(RESET);
            scanf("%lf", &overtime);
            
            setColor(CYAN);
            printf("Enter new bonuses: ");
            setColor(RESET);
            scanf("%lf", &bonuses);

            setColor(CYAN);
            printf("Enter new insurance premium: ");
            setColor(RESET);
            scanf("%lf", &insurance_premium);

            // Update payroll for the next year
            int year = ++employeeList[i].currentYear;
            struct Payroll *payroll = &employeeList[i].payrolls[year];
            payroll->salary = salary;
            payroll->allowances = allowances;
            payroll->overtime = overtime;
            payroll->bonuses = bonuses;
            payroll->insurance_premium = insurance_premium;
            payroll->year = year;

            // Calculate taxes and PF according to the guidelines
            double gross_salary = salary + allowances + overtime + bonuses;
            double tax_rate = (employeeList[i].marital_status[0] == 'M') ? 0.09 : 0.10;  // 9% for married, 10% for single
            double tax_deduction = gross_salary * tax_rate;
            double pf_deduction = gross_salary * 0.10;  // 10% for PF
            double net_salary = gross_salary - tax_deduction - pf_deduction - insurance_premium;

            payroll->tax_deduction = tax_deduction;
            payroll->pf_deduction = pf_deduction;
            payroll->net_salary = net_salary;

            saveEmployeeData();

            setColor(GREEN);
            printf("Employee payroll updated successfully for Year %d!\n", year);
            setColor(RESET);
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void modifyPayrollEntry() {
    int uid;
    double salary, allowances, overtime, bonuses, insurance_premium;

    setColor(CYAN);
    printf("\nModify Entry of Payroll Processing\n=========================\n");
    setColor(RESET);
    
    printf("Enter UID of the employee: ");
    scanf("%d", &uid);

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            int year = employeeList[i].currentYear;
            struct Payroll *payroll = &employeeList[i].payrolls[year];

            setColor(CYAN);
            printf("Enter new monthly salary: ");
            setColor(RESET);
            scanf("%lf", &salary);
            
            setColor(CYAN);
            printf("Enter new allowances: ");
            setColor(RESET);
            scanf("%lf", &allowances);
            
            setColor(CYAN);
            printf("Enter new overtime payment: ");
            setColor(RESET);
            scanf("%lf", &overtime);
            
            setColor(CYAN);
            printf("Enter new bonuses: ");
            setColor(RESET);
            scanf("%lf", &bonuses);

            setColor(CYAN);
            printf("Enter new insurance premium: ");
            setColor(RESET);
            scanf("%lf", &insurance_premium);
            
            // Update current year payroll
            payroll->salary = salary;
            payroll->allowances = allowances;
            payroll->overtime = overtime;
            payroll->bonuses = bonuses;
            payroll->insurance_premium = insurance_premium;

            // Calculate taxes and PF according to the guidelines
            double gross_salary = salary + allowances + overtime + bonuses;
            double tax_rate = (employeeList[i].marital_status[0] == 'M') ? 0.09 : 0.10;  // 9% for married, 10% for single
            double tax_deduction = gross_salary * tax_rate;
            double pf_deduction = gross_salary * 0.10;  // 10% for PF
            double net_salary = gross_salary - tax_deduction - pf_deduction - insurance_premium;

            payroll->tax_deduction = tax_deduction;
            payroll->pf_deduction = pf_deduction;
            payroll->net_salary = net_salary;

            saveEmployeeData();

            setColor(GREEN);
            printf("Employee payroll entry modified successfully for Year %d!\n", year);
            setColor(RESET);
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void viewAllEmployeePayrollSummary() {
    setColor(CYAN);
    printf("\nAll Employee Payroll Summary\n=========================\n");
    setColor(RESET);

    for (int i = 0; i < currentEmployeeCount; i++) {
        struct Employee *employee = &employeeList[i];
        int year = employee->currentYear;
        struct Payroll *payroll = &employee->payrolls[year];
        double annual_salary = payroll->net_salary * 12;

        setColor(GREEN);
        printf("\nPay Slip for %s (Year %d)\n=========================\n", employee->username, year);
        printf("UID: %d\n", employee->uid);
        printf("Basic Salary: %.2f\n", payroll->salary);
        printf("Allowances: %.2f\n", payroll->allowances);
        printf("Overtime: %.2f\n", payroll->overtime);
        printf("Bonuses: %.2f\n", payroll->bonuses);
        printf("Tax Deduction: %.2f\n", payroll->tax_deduction);
        printf("Provident Fund: %.2f\n", payroll->pf_deduction);
        printf("Insurance Premium: %.2f\n", payroll->insurance_premium);
        printf("Net Salary: %.2f\n", payroll->net_salary);
        printf("Annual Salary: %.2f\n", annual_salary);
        setColor(RESET);
    }
}

void searchEmployeePayrollSummary() {
    int uid;
    setColor(CYAN);
    printf("\nSearch Employee Payroll Summary\n=========================\n");
    setColor(RESET);

    printf("Enter UID of the employee: ");
    scanf("%d", &uid);

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            struct Employee *employee = &employeeList[i];
            int year = employee->currentYear;
            struct Payroll *payroll = &employee->payrolls[year];
            double annual_salary = payroll->net_salary * 12;

            setColor(GREEN);
            printf("\nPay Slip for %s (Year %d)\n=========================\n", employee->username, year);
            printf("UID: %d\n", employee->uid);
            printf("Basic Salary: %.2f\n", payroll->salary);
            printf("Allowances: %.2f\n", payroll->allowances);
            printf("Overtime: %.2f\n", payroll->overtime);
            printf("Bonuses: %.2f\n", payroll->bonuses);
            printf("Tax Deduction: %.2f\n", payroll->tax_deduction);
            printf("Provident Fund: %.2f\n", payroll->pf_deduction);
            printf("Insurance Premium: %.2f\n", payroll->insurance_premium);
            printf("Net Salary: %.2f\n", payroll->net_salary);
            printf("Annual Salary: %.2f\n", annual_salary);
            setColor(RESET);
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void auditLogMenu() {
    int option;

    while (1) {
        clearScreen();
        displayLoading();
        setColor(BLUE);
        printf("\nCipherPayroll - Audit Log Menu\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. View All Employee's Audit Log\n");
        printf("2. Search Employee's Audit Log\n");
        printf("3. Back to Admin Menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                viewAllEmployeeAuditLog();
                break;
            case 2:
                searchEmployeeAuditLog();
                break;
            case 3:
                return;
            default:
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}

void viewAllEmployeeAuditLog() {
    setColor(CYAN);
    printf("\nAudit Log for All Employees\n=========================\n");
    setColor(RESET);

    for (int i = 0; i < currentEmployeeCount; i++) {
        struct Employee *employee = &employeeList[i];

        setColor(GREEN);
        printf("\nAudit Log for %s\n=========================\n", employee->username);
        setColor(RESET);

        for (int year = 1; year <= employee->currentYear; year++) {
            struct Payroll *payroll = &employee->payrolls[year];
            double annual_salary = payroll->net_salary * 12;

            setColor(GREEN);
            printf("\nPayroll for Year %d\n-------------------------\n", year);
            printf("Basic Salary: %.2f\n", payroll->salary);
            printf("Allowances: %.2f\n", payroll->allowances);
            printf("Overtime: %.2f\n", payroll->overtime);
            printf("Bonuses: %.2f\n", payroll->bonuses);
            printf("Tax Deduction: %.2f\n", payroll->tax_deduction);
            printf("Provident Fund: %.2f\n", payroll->pf_deduction);
            printf("Insurance Premium: %.2f\n", payroll->insurance_premium);
            printf("Net Salary: %.2f\n", payroll->net_salary);
            printf("Annual Salary: %.2f\n", annual_salary);
            setColor(RESET);
        }
    }
}

void searchEmployeeAuditLog() {
    int uid;
    setColor(BLUE);
    printf("\nSearch Employee Audit Log\n=========================\n");
    setColor(RESET);

    setColor(CYAN);
    printf("Enter UID of the employee to search: ");
    setColor(RESET);
    scanf("%d", &uid);

    for (int i = 0; i < currentEmployeeCount; i++) {
        if (employeeList[i].uid == uid) {
            struct Employee *employee = &employeeList[i];

            setColor(GREEN);
            printf("\nAudit Log for %s\n=========================\n", employee->username);
            setColor(RESET);

            for (int year = 1; year <= employee->currentYear; year++) {
                struct Payroll *payroll = &employee->payrolls[year];
                double annual_salary = payroll->net_salary * 12;

                setColor(GREEN);
                printf("\nPayroll for Year %d\n-------------------------\n", year);
                printf("Basic Salary: %.2f\n", payroll->salary);
                printf("Allowances: %.2f\n", payroll->allowances);
                printf("Overtime: %.2f\n", payroll->overtime);
                printf("Bonuses: %.2f\n", payroll->bonuses);
                printf("Tax Deduction: %.2f\n", payroll->tax_deduction);
                printf("Provident Fund: %.2f\n", payroll->pf_deduction);
                printf("Insurance Premium: %.2f\n", payroll->insurance_premium);
                printf("Net Salary: %.2f\n", payroll->net_salary);
                printf("Annual Salary: %.2f\n", annual_salary);
                setColor(RESET);
            }
            return;
        }
    }

    setColor(RED);
    printf("Error: UID not found!\n");
    setColor(RESET);
}

void displayPayrollGuideline() {
    setColor(CYAN);
    printf("\nEmployee Payroll Guideline\n=========================\n");
    setColor(GREEN);
    printf("1. Basic Salary: Base salary agreed upon employment.\n");
    printf("2. Allowances: Additional compensation (e.g., housing, transportation).\n");
    printf("3. Overtime: Pay for hours worked beyond standard work hours.\n");
    printf("4. Bonuses: Performance-based extra pay.\n");
    printf("5. Tax Deduction: Calculated based on the latest Nepal Government tax policies.\n");
    printf("   - For married employees: 1%% less tax.\n");
    printf("   - For single employees: Standard tax rate.\n");
    printf("6. Provident Fund (PF): Calculated based on the latest Nepal Government PF policies.\n");
    printf("   - Employee contribution: 10%%\n");
    printf("   - Employer contribution: 10%%\n");
    printf("7. Insurance Premium: Fixed amount as agreed.\n");
    printf("8. Net Salary: Gross salary minus tax, PF, and insurance premium.\n");
    setColor(RESET);
}

void encryptionDecryptionTool() {
    int option;

    while (1) {
        clearScreen();
        displayLoading();
        setColor(BLUE);
        printf("\nCipherPayroll - Encryption/Decryption Tool\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. Encryption\n");
        printf("2. Decryption\n");
        printf("3. Back to Admin Menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                encryptionMenu();
                break;
            case 2:
                decryptionMenu();
                break;
            case 3:
                return;
            default:
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}

void encryptFile(const char *filename) {
    FILE *file = fopen(filename, "rb+");
    if (!file) {
        setColor(RED);
        printf("Error: Cannot open file %s for encryption!\n", filename);
        setColor(RESET);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(fileSize);
    if (!buffer) {
        setColor(RED);
        printf("Error: Memory allocation failed!\n");
        setColor(RESET);
        fclose(file);
        return;
    }

    fread(buffer, 1, fileSize, file);
    fseek(file, 0, SEEK_SET);

    for (long i = 0; i < fileSize; i++) {
        buffer[i] ^= XOR_KEY;
    }

    fwrite(buffer, 1, fileSize, file);

    free(buffer);
    fclose(file);
    setColor(GREEN);
    printf("File %s encrypted successfully!\n", filename);
    setColor(RESET);
}

void decryptFile(const char *filename) {
    FILE *file = fopen(filename, "rb+");
    if (!file) {
        setColor(RED);
        printf("Error: Cannot open file %s for decryption!\n", filename);
        setColor(RESET);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(fileSize);
    if (!buffer) {
        setColor(RED);
        printf("Error: Memory allocation failed!\n");
        setColor(RESET);
        fclose(file);
        return;
    }

    fread(buffer, 1, fileSize, file);
    fseek(file, 0, SEEK_SET);

    for (long i = 0; i < fileSize; i++) {
        buffer[i] ^= XOR_KEY;
    }

    fwrite(buffer, 1, fileSize, file);

    free(buffer);
    fclose(file);
    setColor(GREEN);
    printf("File %s decrypted successfully!\n", filename);
    setColor(RESET);
}


void encryptionMenu() {
    int option;

    while (1) {
        clearScreen();
        setColor(BLUE);
        printf("\nCipherPayroll - Encryption Menu\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. Encrypt Employees.bin\n");
        printf("2. Encrypt Login.bin\n");
        printf("3. Encrypt Admin password.txt\n");
        printf("4. Back to Encryption/Decryption Tool Menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                encryptFile("Employees.bin");
                break;
            case 2:
                encryptFile("Login.bin");
                break;
            case 3:
                encryptFile("AdminPassword.txt");
                break;
            case 4:
                return;
            default:
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}

void decryptionMenu() {
    int option;

    while (1) {
        clearScreen();
        setColor(BLUE);
        printf("\nCipherPayroll - Decryption Menu\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. Decrypt Employees.bin\n");
        printf("2. Decrypt Login.bin\n");
        printf("3. Decrypt Admin password.txt\n");
        printf("4. Back to Encryption/Decryption Tool Menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                decryptFile("Employees.bin");
                break;
            case 2:
                decryptFile("Login.bin");
                break;
            case 3:
                decryptFile("AdminPassword.txt");
                break;
            case 4:
                return;
            default:
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}

void adminEmployeeInfoManagement() {
    int option;

    while (1) {
        displayLoading();
        setColor(BLUE);
        printf("\nEmployee Information Management\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. Approve New Employee\n");
        printf("2. Edit Employee Details\n");
        printf("3. View All Employee Details\n");
        printf("4. Change Employee Status\n");
        printf("5. Delete Employee Record\n");
        printf("6. Search Employee Record\n");
        printf("7. Back to Admin Menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                adminApproveNewEmployee();
                break;
            case 2:
                adminEditEmployeeDetails();
                break;
            case 3:
                adminViewAllEmployeeDetails();
                break;
            case 4:
                adminChangeEmployeeStatus();
                break;
            case 5:
                adminDeleteEmployeeRecord();
                break;
            case 6:
                adminSearchEmployeeRecord();
                break;
            case 7:
                return;
            default:
                displayLoading();
                printf("\nEmployee Information Management\n=========================\n");
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
    }
}

void adminMenu() {
    int option;

    while (1) {
        displayLoading();
        setColor(BLUE);
        printf("\nCipherPayroll - Admin Menu\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("1. Employee Information Management\n");
        printf("2. Payroll Processing\n");
        printf("3. Change Admin's Password\n");
        printf("4. Audit Log\n");
        printf("5. Encryption/Decryption Tool\n");
        printf("6. Back to Main Menu\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                adminEmployeeInfoManagement();
                break;
            case 2:
                adminPayrollProcessing();
                break;
            case 3:
                adminChangeAdminPassword();
                break;
            case 4:
                auditLogMenu();
                break;
            case 5:
                encryptionDecryptionTool();
                break;
            case 6:
                return;
            default:
                displayLoading();
                printf("\nAdmin Menu\n=========================\n");
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
        if (!continuePrompt()) {
            break;
        }
    }
}

void mainMenu() {
    int option;
    char adminPassword[PASSWORDLENGTH];

    while (1) {
        displayLoading();
        setColor(BLUE);
        printf("\nCipherPayroll - Employee Payroll System\n=========================\n");
        setColor(RESET);
        setColor(CYAN);
        printf("=========================\n");
        printf("1. Admin (HR)\n");
        printf("2. Client\n");
        printf("3. Exit\n");
        printf("=========================\n");
        setColor(GREEN);
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                clearScreen();
                setColor(BLUE);
                printf("\nAdmin Login\n=========================\n");
                setColor(RESET);
                setColor(CYAN);
                printf("Enter admin password: ");
                setColor(RESET);
                getPasswordInput(adminPassword);
                printf("\n");

                // Validate admin password
                if (strcmp(adminPassword, "") == 0 || !validateAdminPassword(adminPassword)) {
                    setColor(RED);
                    printf("Error: Incorrect admin password!\n");
                    setColor(RESET);
                    printf("\nPress any key to continue...\n");
                    _getch();
                    break;
                }

                adminMenu();
                break;
            case 2:
                mainClientMenu();
                break;
            case 3:
                displayExiting();
                setColor(RESET);
                exit(0);
            default:
                displayLoading();
                printf("\nCipherPayroll - Employee Payroll System\n=========================\n");
                setColor(RED);
                printf("Error: Invalid option!\n");
                setColor(RESET);
                break;
        }
    }
}

