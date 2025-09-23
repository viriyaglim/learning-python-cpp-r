#Hello R
print("Hello Viriya from R in VS Code!")
#Week 1 - This is 30 days learning R

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
library(PerformanceAnalytics)


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

# Get AAPL data from Yahoo Finance
getSymbols("AAPL", src = "yahoo", from = "2023-01-01", to = "2023-12-31")

# AAPL is now an xts object
head(AAPL)

# Daily log returns
aapl_returns <- dailyReturn(AAPL$AAPL.Adjusted, type = "log")
head(aapl_returns)

plot(aapl_returns,
     main = "AAPL Daily Log Returns (2023)",
     col = "blue", ylab = "Log Return")

# Download directly with tq_get
aapl_tq <- tq_get("AAPL", from = "2023-01-01", to = "2023-12-31")

head(aapl_tq)

# Compute log returns
aapl_tq <- aapl_tq %>%
  mutate(log_return = log(adjusted / lag(adjusted)))

# Plot with ggplot2
ggplot(aapl_tq, aes(x = date, y = log_return)) +
  geom_line(color = "steelblue") +
  labs(title = "AAPL Daily Log Returns (2023)",
       x = "Date", y = "Log Return")

# Save returns to CSV
write.csv(aapl_tq, "AAPL_returns.csv", row.names = FALSE)

##-----------------------------------------------
# Download BTC-USD data for 2025
btc <- tq_get("BTC-USD", from = "2025-01-01", to = "2025-09-21")

# Compute daily log returns
btc <- btc %>%
  mutate(log_return = log(adjusted / lag(adjusted)))

# Drop NA (first row)
btc <- btc %>% drop_na()

# Summary statistics
btc %>%
  summarize(
    Mean_Daily_Return = mean(log_return),
    Volatility = sd(log_return),
    Skewness = moments::skewness(log_return),
    Kurtosis = moments::kurtosis(log_return)
  )

# Optional: Plot
ggplot(btc, aes(x = date, y = log_return)) +
  geom_line(color = "darkorange") +
  labs(title = "BTC Daily Log Returns (2025)",
       x = "Date", y = "Log Return")


##---------------------------------------------------------

#Day 4: Basic visualization with ggplot2.

#Plot time series, histograms of returns.

#Task: Compare BTC vs ETH volatility.

# Pull 2025 data
crypto <- tq_get(c("BTC-USD", "ETH-USD"),
                 from = "2025-01-01", to = "2025-09-21")

head(crypto)

#compute log returns
crypto <- crypto %>%
  group_by(symbol) %>%
  mutate(log_return = log(adjusted / lag(adjusted))) %>%
  drop_na()

head(crypto)

#plot time series of returns 
ggplot(crypto, aes(x = date, y = log_return, color = symbol)) +
  geom_line(alpha = 0.7) +
  labs(title = "BTC vs ETH Daily Log Returns (2025)",
       x = "Date", y = "Log Return") +
  theme_minimal()

#histogram comparison
ggplot(crypto, aes(x = log_return, fill = symbol)) +
  geom_histogram(bins = 50, alpha = 0.5, position = "identity") +
  labs(title = "Distribution of BTC vs ETH Returns (2025)",
       x = "Log Return", y = "Frequency") +
  theme_minimal()

#compare volatility 
crypto %>%
  group_by(symbol) %>%
  summarize(
    Mean_Return = mean(log_return),
    Volatility = sd(log_return)
  )



#-----------------------------------------------------------

#Day 5: Portfolio basics.

#Compute portfolio returns, variance, Sharpe ratio.

#Package: PerformanceAnalytics.

# 1) Download prices and compute DAILY LOG RETURNS
stocks <- tq_get(c("AAPL", "MSFT"),
                 from = "2025-01-01", to = "2025-09-22")

returns <- stocks %>%
  group_by(symbol) %>%
  tq_transmute(select     = adjusted,
               mutate_fun = periodReturn,
               period     = "daily",
               type       = "log")            # columns: symbol, date, daily.returns

# 2) Make WIDE table: first column = date (lowercase)
returns_wide <- returns %>%
  select(date, symbol, daily.returns) %>%
  pivot_wider(names_from = symbol, values_from = daily.returns) %>%
  arrange(date)

head(returns_wide)

# 3) Convert to XTS (use lowercase `date`)
returns_xts <- xts(returns_wide %>% select(-date) |> as.matrix(),
                   order.by = as.Date(returns_wide$date))

# 4) Portfolio returns (60% AAPL, 40% MSFT)
weights <- c(0.6, 0.4)

portfolio_returns <- Return.portfolio(returns_xts,
                                      weights = weights,
                                      rebalance_on = "months")
head(portfolio_returns)

# 5) Sharpe ratio (daily; rf = 0). To annualize later, see note below.
SharpeRatio(portfolio_returns, Rf = 0, FUN = "StdDev")

# 6) Portfolio variance (daily) and annualized variance/vol
cov_matrix <- cov(coredata(returns_xts), use = "complete.obs")
port_variance_daily <- as.numeric(t(weights) %*% cov_matrix %*% weights)
port_variance_annual <- port_variance_daily * 252
port_vol_annual <- sqrt(port_variance_annual)

port_variance_daily
port_vol_annual

# 7) Quick performance chart
charts.PerformanceSummary(portfolio_returns,
                          main = "Portfolio Performance — AAPL 60% / MSFT 40% (2025)")

#------------------------------------------------------------------------
#Day 6: Probability & statistics in R.

#Monte Carlo for coin toss & simple stock price model.

set.seed(123)

# Parameters
n_trials <- 1000    # number of experiments
n_flips  <- 10      # flips per experiment

# Simulate coin flips (0 = tails, 1 = heads)
results <- replicate(n_trials, sum(rbinom(n_flips, size = 1, prob = 0.5)))
results
# Distribution of heads in 10 flips
table(results) / n_trials

#visualization
df_coin <- data.frame(heads = results)

ggplot(df_coin, aes(x = heads)) +
  geom_histogram(binwidth = 1, fill = "steelblue", color = "white") +
  labs(title = "Monte Carlo: Coin Toss (10 flips, 1000 trials)",
       x = "Number of Heads", y = "Frequency")

#Monte Carlo Simulation - Stock Price (Geometric Brownian Motion)
set.seed(123)

# Parameters
S0 <- 100       # initial price
mu <- 0.08      # expected annual return (8%)
sigma <- 0.2    # volatility (20%)
T <- 1          # horizon = 1 year
steps <- 252    # daily steps
dt <- T/steps   # time increment
n_sims <- 1000  # number of simulated paths

# Simulate GBM
paths <- matrix(0, nrow = steps+1, ncol = n_sims)
paths[1,] <- S0

for (i in 2:(steps+1)) {
  Z <- rnorm(n_sims)
  paths[i,] <- paths[i-1,] * exp((mu - 0.5*sigma^2)*dt + sigma*sqrt(dt)*Z)
}

# Convert to data frame for plotting
df_paths <- data.frame(time = 0:steps, paths)
df_long <- reshape2::melt(df_paths, id.vars = "time")

# Plot sample of paths
ggplot(df_long[df_long$variable %in% paste0("X", 1:20),], aes(x = time, y = value, group = variable)) +
  geom_line(alpha = 0.6) +
  labs(title = "Monte Carlo Simulation of Stock Prices (GBM)",
       x = "Days", y = "Price")

#Expected Distribution of Ending Prices
final_prices <- paths[steps+1,]

mean(final_prices)      # average final price
sd(final_prices)        # volatility of final price

# Histogram
ggplot(data.frame(final = final_prices), aes(x = final)) +
  geom_histogram(bins = 50, fill = "darkorange", color = "white") +
  labs(title = "Distribution of Final Stock Prices after 1 Year",
       x = "Final Price", y = "Frequency")


#Day 7: Mini-project: Build a return distribution analysis dashboard.

#Shiny Web App Results in a Different Page. Thanks 

