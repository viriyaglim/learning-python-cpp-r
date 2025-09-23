#Hello R
print("Hello Viriya from R in VS Code!")
#This is 30 days learning R

install.packages(c("dplyr", "ggplot2", "tidyr", "quantmod", "tidyquant", "moments", "lubridate", "readr", "readxl", "writexl", "data.table", "zoo", "xts", "TTR", "TSA", "forecast", "tseries", "caret"))
installed.packages()[, "Package"]
library(dplyr)
library(ggplot2)
library(base)
library(tidyr)
library(quantmod)
library(tidyquant)
library(moments)
library(lubridate)
library(readr)
library(readxl)
library(writexl)
library(data.table)
library(zoo)
library(xts)
library(TTR)
library(TSA)
library(forecast)
library(tseries)
library(caret)

#Day 1
#🔹 Week 1: Core R & Data Handling

#Goal: Be comfortable with R syntax, financial datasets, and basic analysis.

#Day 1: R basics (variables, vectors, matrices, functions, apply family).

#Package: base, dplyr

#variables
x <- 5
y <- 10
z <- x + y
print(z)

#vectors

returns <- c(0.01, -0.02, 0.015, 0.005)
mean_return <- mean(returns)
print(mean_return)
sd_return <- sd(returns)
print(sd_return)
length(returns)

#matrices
stock_returns <- matrix(c(0.01, -0.02, 0.015, 0.005,
                         0.02, 0.01, -0.005, 0.01,
                        -0.01, 0.03, 0.02, -0.015), 
                            nrow=4, byrow=TRUE)
colnames(stock_returns) <- c("Stock_A", "Stock_B", "Stock_C")
rownames(stock_returns) <- paste("Day", 1:4)
print(stock_returns)
mean_returns <- colMeans(stock_returns)
print(mean_returns)
sd_returns <- apply(stock_returns, 2, sd)
print(sd_returns)
correlation <- cor(stock_returns)
print(correlation)

#functions
calculate_sharpe <- function(returns, risk_free_rate=0) {
  excess_returns <- returns - risk_free_rate
  sharpe_ratio <- mean(excess_returns) / sd(excess_returns)
  return(sharpe_ratio)
}

daily_returns <- c(0.01, -0.02, 0.015, 0.005)
sharpe <- calculate_sharpe(daily_returns)
print(sharpe)

#apply family
apply(stock_returns, 2, mean) #mean per stock
apply(stock_returns, 2, sd) #volatility per stock

squared_returns <- function(x) { x^2 }
squared_matrix <- apply(stock_returns, 2, squared_returns)
print(squared_matrix)

#Exercise: Simulate returns using rnorm().
#simulation with rnorm()
set.seed(123) #for reproducibility
simulated_returns <- rnorm(1000, mean=0.001, sd=0.02) #1000 days
print(head(simulated_returns))
mean(simulated_returns)
sd(simulated_returns)
sharpe_simulated <- calculate_sharpe(simulated_returns)
print(sharpe_simulated)

#visualization
hist(simulated_returns, breaks=30, main="Simulated Daily Returns", xlab="Returns", col="blue")
boxplot(simulated_returns, main="Boxplot of Simulated Returns", ylab="Returns")


#End of Day 1

#-------------------------------------------------
#Day 2: Data wrangling with dplyr & tidyr.

#Learn mutate, summarize, group_by.

#Practice: Compute mean, volatility, skewness of stock returns.

#example dataset: simulated stock returns
# install packages if not already installed
# install.packages(c("dplyr", "tidyr", "moments"))


library(dplyr)
library(tidyr)
library(moments)

set.seed(123)

# Create dataset (100 days of returns)
data <- data.frame(
  Date = seq(as.Date("2023-01-01"), by = "day", length.out = 100),
  Stock_A = rnorm(100, mean = 0.001, sd = 0.02),
  Stock_B = rnorm(100, mean = 0.0005, sd = 0.015)
)

head(data)

# mutate: add new columns
data <- data %>%
  mutate(
    Portfolio = 0.6 * Stock_A + 0.4 * Stock_B,
    Cumulative_A = cumsum(Stock_A),
    Cumulative_B = cumsum(Stock_B)
  )

head(data)

# summarize: compute summary statistics
data %>%
  summarize(
    Mean_A = mean(Stock_A),
    Volatility_A = sd(Stock_A),
    Skewness_A = skewness(Stock_A),
    Mean_B = mean(Stock_B),
    Volatility_B = sd(Stock_B),
    Skewness_B = skewness(Stock_B)
  )

# group_by: group by month and summarize
data <- data %>%
  mutate(Month = format(Date, "%Y-%m"))

data %>%
  group_by(Month) %>%
  summarize(
    Mean_portfolio = mean(Portfolio),
    Volatility_portfolio = sd(Portfolio),
    Skewness_portfolio = skewness(Portfolio)
  )

# pivot_longer: reshape data from wide to long
data_long <- data %>%
  pivot_longer(cols = c(Stock_A, Stock_B, Portfolio),
               names_to = "Asset",
               values_to = "Return")

head(data_long)



#Day 3: Import/export financial data.

#Package: quantmod, tidyquant

#Task: Download AAPL returns, plot log returns.

