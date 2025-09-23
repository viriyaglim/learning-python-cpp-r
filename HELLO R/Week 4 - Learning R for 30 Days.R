#Hello R
print("Hello Viriya from R in VS Code!")
#Week 4 - This is 30 days learning R

library(dplyr)
library(ggplot2)
library(base)
library(broom)
library(tidyr)
library(plm)
library(quantmod)
library(tidyquant)
library(gmm)
library(rugarch)
library(moments)
library(lmtest)
library(lubridate)
library(boot)
library(rmgarch)
library(readr)
library(car)
library(readxl)
library(writexl)
library(stats4)
library(sandwich)
library(data.table)
library(zoo)
library(xts)
library(TTR)
library(TSA)
library(vars)
library(forecast)
library(tseries)
library(caret)
library(Quandl)
library(PerformanceAnalytics)
library(FFdownload)


# 🔹 Week 4: Simulation & Visualization
# 
# Goal: Apply Monte Carlo, bootstrapping, and create polished reports.
# 
# Day 22: Monte Carlo simulation of stock prices.
# 
# Task: Simulate 10,000 GBM paths.

set.seed(123)  # reproducibility

# Parameters
S0    <- 100      # initial stock price
mu    <- 0.08     # drift (8% annual return)
sigma <- 0.20     # volatility (20% annual)
T     <- 1        # time horizon = 1 year
N     <- 252      # steps (daily)
M     <- 10000    # number of simulations

dt <- T / N       # time step

# 1) Simulate paths
# matrix of random shocks: M simulations x N steps
Z <- matrix(rnorm(M * N), nrow = M, ncol = N)

# GBM increments
increments <- (mu - 0.5 * sigma^2) * dt + sigma * sqrt(dt) * Z

# cumulative sum across time
log_paths <- t(apply(increments, 1, cumsum))

# add initial log-price
log_paths <- cbind(0, log_paths)  # add time 0
S_paths <- S0 * exp(log_paths)    # exponentiate to get stock prices

# 2) Prepare data for plotting (just first 50 paths for clarity)
time_points <- 0:N
plot_df <- data.frame(time = rep(time_points, M),
                      sim  = rep(1:M, each = N+1),
                      price = as.vector(t(S_paths)))

plot_subset <- plot_df %>% filter(sim <= 50)

# 3) Plot a subset of paths
ggplot(plot_subset, aes(x = time, y = price, group = sim)) +
  geom_line(alpha = 0.6, color = "steelblue") +
  labs(title = "Monte Carlo Simulation of Stock Prices (GBM)",
       x = "Time step (days)", y = "Stock Price") +
  theme_minimal()

# 4) Distribution of terminal prices (at T=1 year)
terminal_prices <- S_paths[, N+1]

ggplot(data.frame(terminal = terminal_prices), aes(x = terminal)) +
  geom_histogram(bins = 50, fill = "darkred", alpha = 0.7) +
  labs(title = "Distribution of Terminal Stock Prices (1 Year)",
       x = "Price at T=1", y = "Frequency") +
  theme_minimal()

# 5) Summary statistics
cat("\n--- Terminal Price Summary ---\n")
cat("Mean terminal price :", mean(terminal_prices), "\n")
cat("Median terminal price :", median(terminal_prices), "\n")
cat("Std Dev terminal price :", sd(terminal_prices), "\n")



# ------------------------------ 
# Day 23: Bootstrapping.
# 
# Package: boot.
# 
# Task: Bootstrap Sharpe ratio confidence intervals.

# ---------- 1) Download BTC data ----------
btc_tbl <- tq_get("BTC-USD", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  transmute(date, price = adjusted)

btc_ret_tbl <- btc_tbl %>%
  mutate(ret = log(price / dplyr::lag(price))) %>%
  tidyr::drop_na()

ret_vec <- btc_ret_tbl$ret

# ---------- 2) Sharpe ratio function ----------
sharpe_fun <- function(r) {
  mean(r) / sd(r)   # assuming risk-free rate ~ 0
}

# Sample Sharpe
sharpe_sample <- sharpe_fun(ret_vec)
cat("Sample Sharpe ratio:", sharpe_sample, "\n")

# ---------- 3) Bootstrap function ----------
boot_sharpe <- function(data, indices) {
  r_star <- data[indices]    # resampled returns
  return(sharpe_fun(r_star))
}

# ---------- 4) Run bootstrap ----------
set.seed(123)
boot_results <- boot(data = ret_vec, statistic = boot_sharpe, R = 5000)

# ---------- 5) Confidence intervals ----------
ci_percentile <- boot.ci(boot_results, type = c("perc", "bca"))

cat("\nBootstrap Sharpe Ratio Results:\n")
print(ci_percentile)

# ---------- 6) Plot bootstrap distribution ----------
boot_df <- data.frame(sr = boot_results$t)

hist(boot_df$sr, breaks = 50, col = "skyblue", border = "white",
     main = "Bootstrap Distribution of Sharpe Ratio",
     xlab = "Sharpe Ratio")
abline(v = sharpe_sample, col = "red", lwd = 2)



# -------------------------------------- 
# Day 24: Value-at-Risk (VaR) simulation.
# 
# Historical, parametric, and Monte Carlo VaR.

# ---------- 1) Download BTC daily returns ----------
btc_tbl <- tq_get("BTC-USD", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  transmute(date, price = adjusted)

btc_ret_tbl <- btc_tbl %>%
  mutate(ret = log(price / dplyr::lag(price))) %>%
  tidyr::drop_na()

ret_xts <- xts(btc_ret_tbl$ret, order.by = btc_ret_tbl$date)

# ---------- 2) Define confidence levels ----------
p_levels <- c(0.95, 0.99)

# ---------- 3) Compute VaR & ES for each level ----------
var_list <- c()
es_list  <- c()

for (p in p_levels) {
  var_val <- VaR(R = ret_xts, p = p, method = "historical")  # FIXED: use p directly
  es_val  <- ES(R = ret_xts,  p = p, method = "historical")
  
  var_list <- c(var_list, as.numeric(var_val))
  es_list  <- c(es_list,  as.numeric(es_val))
}

# ---------- 4) Combine results ----------
risk_df <- data.frame(
  Confidence = rep(p_levels, times = 2),
  Measure    = rep(c("VaR", "ES"), each = length(p_levels)),
  Value      = c(var_list, es_list)
)

print(risk_df)

# ---------- 5) Visualization ----------
ggplot(risk_df, aes(x = factor(Confidence), y = Value, fill = Measure)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(title = "BTC-USD Risk Measures: VaR vs Expected Shortfall",
       x = "Confidence Level", y = "Daily Return Threshold") +
  theme_minimal()


# -------------------------------------------
# Day 26: Advanced visualization.
# 
# Interactive dashboards with shiny.


#Shown in Exercise_Week4_Day26 

# -------------------------------------------
# Day 27: Report automation.
# 
# Markdown/Quarto reports with charts & tables.
# Shown in Quarto_Exercise_Week4_Day27.qmd 
#---------------------------------------------

# Day 28: Integrating models.
# 
# Combine regression + GARCH + VaR into one workflow.

#THIS Results in Error 

get_logrets <- function(symbol, from = "2023-01-01", to = "2023-12-31") {
  tq_get(symbol, from = from, to = to) %>%
    arrange(date) %>%
    tq_transmute(
      select      = adjusted,
      mutate_fun  = periodReturn,
      period      = "daily",
      type        = "log",
      col_rename  = "ret"
    ) %>%
    mutate(ret = as.numeric(ret)) %>%
    drop_na()
}

# 3) Fetch returns
btc_ret_tbl <- get_logrets("BTC-USD")
spx_ret_tbl <- get_logrets("^GSPC")

# 4) Merge by date and keep only needed columns (avoid rename clashes)
data_tbl <- inner_join(
  btc_ret_tbl %>% transmute(date, btc_ret = ret),
  spx_ret_tbl %>% transmute(date, spx_ret = ret),
  by = "date"
) %>%
  filter(is.finite(btc_ret), is.finite(spx_ret))

stopifnot(nrow(data_tbl) > 50)   # sanity check

# xts for risk measures
btc_xts <- xts(data_tbl$btc_ret, order.by = data_tbl$date)

# 5) Regression (CAPM-style): BTC ~ SPX
reg_fit <- lm(btc_ret ~ spx_ret, data = data_tbl)
print(summary(reg_fit))

# Residuals aligned to dates
resid_df <- tibble(
  date  = data_tbl$date,
  resid = as.numeric(residuals(reg_fit))
)

# 6) Fit GARCH(1,1) on centered residuals (robust solver)
resid_vec <- scale(resid_df$resid, center = TRUE, scale = FALSE)[, 1]

garch_spec <- ugarchspec(
  variance.model     = list(model = "sGARCH", garchOrder = c(1, 1)),
  mean.model         = list(armaOrder = c(0, 0), include.mean = FALSE),
  distribution.model = "std"
)

garch_fit <- ugarchfit(
  spec = garch_spec,
  data = resid_vec,
  solver = "hybrid",
  fit.control = list(scale = 1)
)
show(garch_fit)

# Conditional volatility aligned with dates
cond_tbl <- tibble(
  date     = resid_df$date,
  cond_vol = as.numeric(sigma(garch_fit))
)

# 7) VaR & ES on BTC returns (historical; tail prob p)
var_95 <- VaR(btc_xts, p = 0.05, method = "historical")
es_95  <- ES (btc_xts, p = 0.05, method = "historical")

risk_tbl <- data.frame(
  Measure = c("VaR(95%)", "ES(95%)"),
  Value   = c(as.numeric(var_95), as.numeric(es_95))
)
print(risk_tbl)

# 8) Plots
# a) Regression fit
p1 <- ggplot(data_tbl, aes(x = spx_ret, y = btc_ret)) +
  geom_point(alpha = 0.5) +
  geom_smooth(method = "lm", color = "blue", se = FALSE) +
  labs(title = "BTC vs S&P 500 (Daily Log Returns)",
       x = "S&P 500 Return", y = "BTC Return") +
  theme_minimal()

# b) GARCH conditional volatility of residuals
p2 <- ggplot(cond_tbl, aes(x = date, y = cond_vol)) +
  geom_line(color = "red") +
  labs(title = "GARCH(1,1) Conditional Volatility of Regression Residuals",
       x = "Date", y = "Volatility (σ_t)") +
  theme_minimal()

print(p1)
print(p2)


# Day 29: Mini-project: Build a “Quant Risk Report” for a stock/crypto.
# Shown in Mini Project_Quant Risk Report_Quarto_Week4_Day29.qmd


# Day 30: Capstone project.
# 
# Choose one:
#   
# Full Fama-French analysis on a portfolio.
# 
# Volatility forecasting for BTC.
# 
# Monte Carlo VaR dashboard in Shiny.