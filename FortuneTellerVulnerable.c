#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_FORTUNES 5
#define SMALL_BUFFER 16

const char *fortunes[] = {
    "You will find great wealth!",
    "Love is on the horizon.",
    "A journey awaits you.",
    "Beware of false friends.",
    "Success is within reach!"
};

typedef struct {
    char name[SMALL_BUFFER];
    int lucky_number;
    char *personal_fortune;
} UserProfile;

// VULNERABILITY 9: Null pointer dereference setup
UserProfile *create_profile(const char *name) {
    UserProfile *profile = (UserProfile *)malloc(sizeof(UserProfile));
    
    // VULNERABILITY 4: Type mismatch - strcpy expects char*, returns char*
    // but we're ignoring return and using int-sized variable conceptually
    int result;
    result = (int)strcpy(profile->name, name);  // Type mismatch in cast
    
    // VULNERABILITY 3: Unsafe function - strcpy with no bounds checking
    // Also causes VULNERABILITY 2: Buffer overflow if name > 15 chars
    strcpy(profile->name, name);
    
    profile->personal_fortune = NULL;
    return profile;
}

void set_personal_fortune(UserProfile *profile, const char *fortune) {
    profile->personal_fortune = (char *)malloc(strlen(fortune) + 1);
    strcpy(profile->personal_fortune, fortune);
}

void free_profile(UserProfile *profile) {
    free(profile->personal_fortune);
    free(profile);
}

int calculate_lucky_number(int birth_year, int multiplier) {
    // VULNERABILITY 6: Integer overflow
    // If birth_year and multiplier are large, this overflows
    int huge_calc = birth_year * multiplier * 99999;
    return huge_calc % 100;
}

void print_fortune_banner(char *user_input) {
    char banner[64];
    
    // VULNERABILITY 5: Format string vulnerability
    // User input used directly as format string
    printf(user_input);
    printf("\n");
    
    // VULNERABILITY 3 (again): Unsafe strcat
    strcpy(banner, "=== Fortune for: ");
    strcat(banner, user_input);  // Can overflow banner buffer
    strcat(banner, " ===");
    printf("%s\n", banner);
}

int get_fortune_index(int max_index) {
    // VULNERABILITY 7: Off-by-one error
    // Should be rand() % max_index, but uses <= causing access beyond array
    int index = rand() % (max_index + 1);  // Can return max_index (out of bounds)
    return index;
}

void demonstrate_memory_bugs(void) {
    char *temp = (char *)malloc(32);
    strcpy(temp, "Temporary fortune");
    
    printf("Temp: %s\n", temp);
    
    free(temp);
    
    // VULNERABILITY 8: Double free
    free(temp);
    
    // VULNERABILITY 10: Use after free
    printf("After free: %s\n", temp);
}

int main(int argc, char *argv[]) {
    // VULNERABILITY 1: Uninitialized variable
    int user_choice;  // Never initialized before potential use
    int birth_year;
    char name_buffer[256];
    UserProfile *current_user;
    UserProfile *null_user = NULL;  // For null pointer demo
    
    srand(time(NULL));
    
    printf("╔════════════════════════════════════╗\n");
    printf("║   MYSTIC FORTUNE TELLER v1.0       ║\n");
    printf("╚════════════════════════════════════╝\n\n");
    
    printf("Enter your name: ");
    fgets(name_buffer, sizeof(name_buffer), stdin);
    name_buffer[strcspn(name_buffer, "\n")] = 0;
    
    // This will overflow if name > 15 chars (VULNERABILITY 2)
    current_user = create_profile(name_buffer);
    
    // VULNERABILITY 5: Format string - passing user input as format
    print_fortune_banner(name_buffer);
    
    printf("\nEnter your birth year: ");
    scanf("%d", &birth_year);
    getchar();  // consume newline
    
    // VULNERABILITY 6: Integer overflow with large multiplier
    current_user->lucky_number = calculate_lucky_number(birth_year, INT_MAX / 100);
    printf("Your lucky number is: %d\n", current_user->lucky_number);
    
    // VULNERABILITY 7: Off-by-one - may access fortunes[5] which doesn't exist
    int fortune_idx = get_fortune_index(MAX_FORTUNES);
    
    // Potential crash here due to off-by-one
    if (fortune_idx < MAX_FORTUNES + 1) {  // Wrong bounds check
        printf("\n🔮 Your fortune: %s\n", fortunes[fortune_idx]);
    }
    
    // VULNERABILITY 1: Using uninitialized user_choice
    // This path might execute with garbage value
    if (argc > 1) {
        // user_choice was never set but we use it
        if (user_choice == 42) {
            printf("Secret mode activated!\n");
        }
    }
    
    // VULNERABILITY 9: Null pointer dereference
    printf("\nAttempting to access null user profile...\n");
    // This will crash - dereferencing NULL
    // printf("Null user name: %s\n", null_user->name);  
    // Uncomment above to trigger crash
    
    // Demonstrating the null check that SHOULD be done
    if (null_user != NULL) {
        printf("Null user name: %s\n", null_user->name);
    } else {
        // Intentionally dereference anyway for demonstration
        // VULNERABILITY 9: Null pointer dereference
        char *bad_access = null_user->name;  // Compiles but crashes at runtime
        (void)bad_access;  // Suppress unused warning
    }
    
    printf("\n--- Memory Bug Demonstration ---\n");
    demonstrate_memory_bugs();  // Contains double-free and use-after-free
    
    // Clean up
    free_profile(current_user);
    
    // VULNERABILITY 8 & 10: Double free and use-after-free on main profile
    printf("\nFinal fortune from freed memory: ");
    printf("%s\n", current_user->name);  // Use after free
    free_profile(current_user);  // Double free
    
    return 0;
}
