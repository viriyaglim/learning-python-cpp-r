#A2
library(bizdays)
print("Hello, I am Viriya Gunawan Lim. Welcome to A2.")  

#Problem 1.2: Financial Calendar Functions
print("This is for problem 1.2.")
#GIVEN HINT 
# Date formatting for weekday extraction
format(as.Date("2024-10-01"), "%a")  # Returns "Tue"
# Efficient date sequence generation
start_date <- as.Date(paste0(year, "-01-01"))
end_date <- as.Date(paste0(year + 1, "-01-01")) - 1
date_sequence <- seq(start_date, end_date, by = "day")


#Task A <- Leap Year 
is_leap_year <- function(year) {
  (year %% 4  == 0 & year %% 100 != 0) | (year %% 400 == 0)
} #(divisible by 4 leap and divisible by 100 not leap) (400 leap)


# GIVEN, Sample: Validation tests for leap year functions
cat("Testing leap year functions:\n")
cat("2024 (should be TRUE):", is_leap_year(2024), "\n")
cat("2023 (should be FALSE):", is_leap_year(2023), "\n")
cat("2000 (should be TRUE):", is_leap_year(2000), "\n")
cat("1900 (should be FALSE):", is_leap_year(1900), "\n")



#Task B <- Monthly Date 28 Settlement
monthly_settlement_weekdays <- function(year) {
  months <- 1:12
  #GIVEN from hint 
  dates <- as.Date(paste0(year, "-", sprintf("%02d", months), "-28"))
  weekdays <- format(dates, "%a")  
  
  data.frame(
    month   = months,
    date = dates,
    weekday = weekdays,
    row.names = NULL
  )
}

#Test on 2025
monthly_settlement_weekdays(2025)

#Task C <- Business Days Counter 
count_business_days <- function(year) {
  # GIVEN hint
  start_date    <- as.Date(paste0(year, "-01-01"))
  end_date      <- as.Date(paste0(year + 1, "-01-01")) - 1
  date_sequence <- seq(start_date, end_date, by = "day")
  
  # Days as numbers with Mon=1, ..., Sun=7
  daysasnum <- as.integer(format(date_sequence, "%u"))
  
  # Count only business days (Mon-Fri)
  sum(daysasnum <= 5)
}


# Test for year calculations
count_business_days(2023)
count_business_days(2024)
count_business_days(2025)

# Compare with bizdays package 
create.calendar("MyCalendar", weekdays=c("saturday", "sunday"))

bizdays("2023-01-01", "2024-01-01", cal = "MyCalendar")  # -> 260
bizdays("2024-01-01", "2025-01-01", cal = "MyCalendar")  # -> 262
bizdays("2025-01-01", "2026-01-01", cal = "MyCalendar")  # -> 261

a <- (count_business_days(2023) == bizdays("2023-01-01", "2024-01-01", cal = "MyCalendar"))
b <- (count_business_days(2024) == bizdays("2024-01-01", "2025-01-01", cal = "MyCalendar"))
c <- (count_business_days(2025) == bizdays("2025-01-01", "2026-01-01", cal = "MyCalendar"))
print(a)
print(b)
print(c)
      
#OKay, The results match between the function and bizdays package.


#Problem 1.3 <- Optimal Stopping Theory 
print("This is for problem 1.3.")

set.seed(42)

#Step 1: Core Simulation Function

#AI Usage : ChatGPT guided me through the steps
make_choice <- function(N, split_number) {
  stopifnot(N >= 2, split_number >= 0, split_number < N)
  
  # Random arrival of qualities (1..N, N is the best)
  seq_scores <- sample(1:N, size = N, replace = FALSE)
  
  # Observation phase: look at first k candidates, take the maximum as threshold
  if (split_number == 0) {
    threshold <- -Inf
  } else {
    threshold <- max(seq_scores[1:split_number])
  }
  
  # Select the first who beats the threshold
  pick_index <- NA_integer_
  if (split_number < N) {
    # scan from k+1 .. N for first score > threshold
    for (i in (split_number + 1):N) {
      if (seq_scores[i] > threshold) { pick_index <- i; break }
    }
  }
  
  # if none, pick the last candidate
  if (is.na(pick_index)) pick_index <- N
  
  # Success if we picked the absolute best (score == N)
  return(seq_scores[pick_index] == N)
}

# estimate success probability via Monte Carlo
simulate_success_rate <- function(N, k, trials = 5000L) {
  # Vectorized replicate 
  mean(replicate(trials, make_choice(N, k)))
}

# Step 2: Optimization

find_optimal <- function(N, trials = 5000L) {
  stopifnot(N >= 2)
  k_grid <- 1:max(1, floor(N/2)) #check N/2 
  
  rates <- vapply(k_grid, function(k) simulate_success_rate(N, k, trials), numeric(1))
  best_idx <- which.max(rates)
  list(
    N = N,
    k_grid = k_grid,
    rates = rates,
    best_k = k_grid[best_idx],
    best_prob = rates[best_idx]
  )
}

# Checking for N = 3, 10,100
Ns <- c(3, 10, 100)
trials <- 5000L

results <- lapply(Ns, function(n) find_optimal(n, trials = trials))

# Check results 
for (res in results) {
  theo_k <- floor(res$N / exp(1))  # ≈ N/e reference
  cat(sprintf("\nN = %d\n", res$N))
  cat(sprintf("Empirical optimal k = %d, success ≈ %.3f\n", res$best_k, res$best_prob)) #round to 3 dec
  cat(sprintf("Theoretical k ≈ floor(N/e), %d (ref)\n", theo_k))
}

#AI Usage: ChatGPT helped me to setup the visualization
# Visualization
op <- par(mfrow = c(1, length(Ns)), mar = c(4, 4, 3, 1))
on.exit(par(op), add = TRUE)

for (res in results) {
  theo_k <- floor(res$N / exp(1))
  plot(
    res$k_grid, res$rates, type = "b", pch = 19,
    xlab = "Split size k (observe-only)",
    ylab = "Success probability",
    main = paste0("N = ", res$N)
  )
  abline(v = res$best_k, lty = 2)     # for empirical k
  abline(v = theo_k, lty = 3)         # for the theoretical k 
  legend(
    "bottomright",
    legend = c(
      sprintf("Empirical k* = %d (%.3f)", res$best_k, res$best_prob),
      sprintf("Theoretical ≈ N/e = %d", theo_k)
    ),
    lty = c(2, 3), bty = "n", cex = 0.9
  )
}


#Exercism Medium Level: 
#All Your Base
#Instruction Given: Convert a sequence of digits in one base, representing a number, into a sequence of digits in another base, representing the same number.

print("This is for the Exercism Medium Level: All Your Base")

convert_base <- function(digits, from_base, to_base) {
  # Checking input first
  if (from_base < 2 || to_base < 2) {
    stop("Bases must be at least 2.")
  }
  if (any(digits < 0) || any(digits >= from_base)) {
    stop("All digits must be non-negative and less than the 'from' base.")
  }
  
  # Convert from the original base to decimal
  decimal_value <- sum(digits * (from_base ^ (length(digits) - seq_along(digits))))
  
  #Base case: number zero
  if (decimal_value == 0) {
    return(c(0))
  }
  
  # Conversion to the target base
  result <- c() #initialize 
  while (decimal_value > 0) {
    remainder <- decimal_value %% to_base
    result <- c(remainder, result)
    decimal_value <- decimal_value %/% to_base
  }
  
  return(result)
}
#Just Testing
convert_base(c(1, 0, 1), 2, 10)  # Base 2 to 10 (Binary to Decimal)
convert_base(c(5, 2), 10, 16)    # Base 10 to 16
convert_base(c(2, 1), 3, 10)     # Base 3 to 10
convert_base(c(1, 0, 1), 2, 3)   # Base 2 to 3



#End of the code 



print("End of the code.")


