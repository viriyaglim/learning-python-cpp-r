#Hello R
print("Hello Viriya from R in VS Code!")
#Week 2 - This is 30 days learning R

library(dplyr)
library(ggplot2)
library(base)
library(broom)
library(tidyr)
library(plm)
library(quantmod)
library(tidyquant)
library(gmm)
library(moments)
library(lmtest)
library(lubridate)
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
library(forecast)
library(tseries)
library(caret)
library(Quandl)
library(PerformanceAnalytics)
library(FFdownload)

# 🔹 Week 2: Econometrics Foundations
# 
# Goal: Apply regression & estimation tools in finance.
# 
# Day 8: Linear regression in R (lm).
# 
# Task: CAPM regression of AAPL on S&P500.
# Download daily prices for 2023
#AAPL and S&P 500 Data 
prices <- tq_get(c("AAPL", "^GSPC"),
                 from = "2023-01-01", to = "2023-12-31")

head(prices)

#Daily Log Returns
returns <- prices %>%
  group_by(symbol) %>%
  tq_transmute(select     = adjusted,
               mutate_fun = periodReturn,
               period     = "daily",
               type       = "log")

head(returns)

#Merge into one dataset
returns_wide <- returns %>%
  pivot_wider(names_from = symbol, values_from = daily.returns) %>%
  rename(AAPL_ret = AAPL, MKT_ret = `^GSPC`) %>%
  drop_na()

head(returns_wide)

#capm regression
# lm(Y ~ X)
capm_model <- lm(AAPL_ret ~ MKT_ret, data = returns_wide)

#intercept = alpha, 
#slope = beta
summary(capm_model)

#estimates 
tidy(capm_model)

#visualization
ggplot(returns_wide, aes(x = MKT_ret, y = AAPL_ret)) +
  geom_point(alpha = 0.5, color = "steelblue") +
  geom_smooth(method = "lm", se = FALSE, color = "darkred") +
  labs(title = "CAPM Regression: AAPL vs S&P500",
       x = "Market Return (S&P500)", y = "AAPL Return")


# ------------------------------------------
# Day 9: Hypothesis testing (t-test, F-test, Wald test).
# 
# Package: lmtest.
# CAPM regression from before
capm_model <- lm(AAPL_ret ~ MKT_ret, data = returns_wide)
summary(capm_model)
#reject H0, AAPL related to the market 

#t test (individual significant coefficient)
# test if beta = 1 (instead of 0)
coeftest(capm_model, vcov = vcov(capm_model))   # usual t-tests
#reject H0, model significant, 


waldtest(capm_model, . ~ 1, vcov = vcov(capm_model))  # drops regressor

# F-test on the overall model significance
f_test <- waldtest(capm_model)
f_test

##waldtest(model_with_factors, . ~ 1)   # joint significance of all factors


linearHypothesis(capm_model, "MKT_ret = 1")
#fail to reject H0 since 0.12 > 0.05

#Beta ≈ 1.10 → AAPL is slightly more volatile than the market, but not significantly different from 1.
#Alpha not significant → AAPL does not generate abnormal returns beyond what the market explains.
#Model is highly significant overall → S&P500 explains a large portion of AAPL’s returns (R² ≈ 0.52).

# --------------------------------------------------
# Day 10: Maximum Likelihood Estimation (MLE).
# 
# Package: stats4
# 
# Task: Estimate normal distribution parameters of returns.

# Get AAPL data
aapl <- tq_get("AAPL", from = "2023-01-01", to = "2023-12-31") %>%
  mutate(ret = log(adjusted / lag(adjusted))) %>%
  drop_na()

returns <- aapl$ret

#define log-likehood function
loglik_normal <- function(mu, sigma) {
  n <- length(returns)
  -sum(dnorm(returns, mean = mu, sd = sigma, log = TRUE))
}

#estimate with mle()
mle_fit <- mle(loglik_normal, 
               start = list(mu = 0, sigma = 0.01),  # initial guesses
               method = "L-BFGS-B",
               lower = c(-1, 1e-6))                 # sigma > 0 constraint

summary(mle_fit)

#extract estimates 
coef(mle_fit)     # estimated parameters
vcov(mle_fit)     # variance-covariance matrix

#compare with sample estimates 

mean(returns)     # sample mean
sd(returns)       # sample standard deviation

#You should find that the MLE estimates match the sample mean & sd (because for normal distribution, the MLE = sample estimates).


#-------------------------------------------------------------- 

# Day 11: Generalized Method of Moments (GMM).
# 
# Package: gmm.
# 
# Task: Estimate moment conditions for CAPM.


# Download prices AAPL and S&P 500
prices <- tq_get(c("AAPL", "^GSPC"),
                 from = "2023-01-01", to = "2023-12-31")

# Compute daily log returns
returns <- prices %>%
  group_by(symbol) %>%
  tq_transmute(select     = adjusted,
               mutate_fun = periodReturn,
               period     = "daily",
               type       = "log")

# Wide format
returns_wide <- returns %>%
  pivot_wider(names_from = symbol, values_from = daily.returns) %>%
  rename(AAPL = AAPL, MKT = `^GSPC`) %>%
  drop_na()


#CAPM moment conditions
# Moment conditions function
gCAPM <- function(theta, data) {
  alpha <- theta[1]
  beta  <- theta[2]
  
  Ri <- data[,1]   # AAPL returns
  Rm <- data[,2]   # Market returns
  
  eps <- Ri - alpha - beta * Rm
  
  g1 <- eps                   # E[eps] = 0
  g2 <- eps * Rm              # E[eps*Rm] = 0
  
  cbind(g1, g2)
}


#run gmm estimation
# Data matrix: (AAPL, Market)
data_mat <- as.matrix(returns_wide[, c("AAPL", "MKT")])

# Initial guesses
theta0 <- c(0, 1)

# GMM estimation
gmm_fit <- gmm(gCAPM, x = data_mat, t0 = theta0)

summary(gmm_fit)

# 1. Alpha (θ₁ = 0.000774)
# 
# Daily abnormal return ≈ 0.077% per day.
# 
# p-value = 0.2286 > 0.05 → not statistically significant.
# 👉 No evidence that AAPL delivers returns beyond what the market explains.
# 
# 2. Beta (θ₂ = 1.1013)
# 
# Estimate: 1.10 → AAPL is slightly more volatile than the market.
# 
# p-value ≈ 0 → highly significant.
# 👉 Strong evidence that AAPL moves with the market, with slightly higher sensitivity.
# 
# 3. J-Test of Overidentifying Restrictions
# 
# Degrees of freedom = 0 → model is just-identified (same number of moments as parameters).
# 
# J-test cannot reject anything → CAPM fits exactly given the imposed moments.
# 
# ✅ Combined Result
# 
# Alpha ≈ 0, not significant → CAPM holds, no abnormal returns.
# 
# Beta ≈ 1.10, significant → AAPL’s returns are strongly linked to S&P500, slightly riskier than the market.
# 
# Model validity: Since the system is just-identified, the CAPM assumptions can’t be rejected here.

# ----------------------------------------------------------
# Day 12: Panel data econometrics.
# 
# Package: plm.
# 
# Task: Run fixed effects regression on firm returns.
# Example tickers
tickers <- c("AAPL", "MSFT", "AMZN")

# Download prices
prices <- tq_get(tickers, from = "2023-01-01", to = "2023-12-31")

# Compute daily log returns
returns <- prices %>%
  group_by(symbol) %>%
  tq_transmute(select     = adjusted,
               mutate_fun = periodReturn,
               period     = "daily",
               type       = "log")

head(returns)

# Market (S&P500)
mkt <- tq_get("^GSPC", from = "2023-01-01", to = "2023-12-31") %>%
  tq_transmute(select = adjusted,
               mutate_fun = periodReturn,
               period = "daily",
               type = "log") %>%
  rename(MKT = daily.returns)

# Merge firm & market returns
panel_data <- left_join(returns, mkt, by = "date") %>%
  drop_na()

head(panel_data)

# Define panel structure: firm (symbol) and time (date)
pdata <- pdata.frame(panel_data, index = c("symbol", "date"))

# Fixed effects regression
capm_fe <- plm(daily.returns ~ MKT, data = pdata, model = "within")

summary(capm_fe)

# Random effects for comparison
capm_re <- plm(daily.returns ~ MKT, data = pdata, model = "random")

summary(capm_re)

#hausman test FE, RE
phtest(capm_fe, capm_re)


# FE Interpretation
# Beta (1.27) → On average, the three firms are 27% more volatile than the market.
# 
# Highly significant (p < 2e-16) → market returns strongly explain firm returns.
# 
# No intercept reported (because FE removes firm-specific intercepts).
# 
# R² ≈ 0.40 → 40% of variation in returns is explained by the market.
# 
# 👉 Interpretation: firms’ αᵢ (intercepts) may differ, but the slope (β) is common across firms.

# RE Interpretation
# Beta (1.27) → same as FE.
# 
# Intercept ~0.0008 (weakly significant at 10% level).
# 
# Variance of individual effects = 0 → suggests there’s no real cross-sectional heterogeneity across firms.

#Hausman Test 
# Null hypothesis (H₀): RE is consistent & efficient.
# 
# Alternative (H₁): FE is consistent, RE is inconsistent.
# 
# Since p = 1 > 0.05, we fail to reject H₀ → Random Effects is valid.
# 
# 👉 That means RE is preferred here, since FE and RE give almost identical results and there is no evidence of bias.



# ----------------------------------------------------------
# Day 13: Model diagnostics.
# 
# Heteroskedasticity test (White, Breusch-Pagan), autocorrelation (Durbin-Watson).
# Using pdata from Day 12
capm_fe <- plm(daily.returns ~ MKT, data = pdata, model = "within")
capm_re <- plm(daily.returns ~ MKT, data = pdata, model = "random")

#Breusch–Pagan test (heteroskedasticity)
bptest(capm_re)   # works like on lm objects

# H₀: homoskedastic errors
# 
# H₁: heteroskedastic errors
# 
# If p < 0.05 → reject H₀ → heteroskedasticity present.

#RESULTS: 
# H₀: Homoskedasticity (constant error variance).
# 
# p = 0.1794 > 0.05 → fail to reject H₀.
# ✅ No evidence of heteroskedasticity.

# White test (robust heteroskedasticity test)
bptest(capm_re, ~ MKT + I(MKT^2), data = pdata)
# 
# More general than Breusch–Pagan.
# 
# Detects nonlinear forms of heteroskedasticity.

#RESULTS: 
# p = 0.3157 > 0.05 → again fail to reject H₀.
# ✅ Confirms no heteroskedasticity problem.

# Durbin–Watson test (autocorrelation)
dwtest(capm_re)

# H₀: no first-order autocorrelation
# 
# If p < 0.05 → reject H₀ → evidence of autocorrelation.

#RESULTS: 
# DW test didn’t return a valid statistic for your plm object.
# 
# For panel models, use pbgtest(capm_re) (Breusch–Godfrey panel autocorrelation test).
# 👉 Current result is inconclusive → can’t say if there’s autocorrelation from DW.

# Robust Standard Errors (if diagnostics fail)
# If heteroskedasticity or autocorrelation is detected, use robust covariance estimators:
#   
coeftest(capm_re, vcov = vcovHC(capm_re, type = "HC1"))   # robust SE (White)
# Even with robust SEs, the coefficients remain highly significant.
# 
# This shows your estimates are stable and robust to potential heteroskedasticity.


#Economically: The CAPM regression is well-behaved → no major specification or error structure issues.

# -------------------------------------------------------------
# Day 14: Mini-project: Estimate CAPM & Fama-French 3 factors, test significance.

#Really Error When Run

# # 1) Stock returns (daily)
# # -------------------------
# tickers <- c("AAPL","MSFT","AMZN")
# 
# prices <- tq_get(tickers, from = "2023-01-01", to = "2023-12-31")
# 
# returns <- prices %>%
#   group_by(symbol) %>%
#   tq_transmute(
#     select     = adjusted,
#     mutate_fun = periodReturn,
#     period     = "daily",
#     type       = "log"
#   ) %>%
#   dplyr::rename(ret = daily.returns)
# 
# # -------------------------
# # 2) Fama–French 3 factors (daily) via FFdownload
# # -------------------------
# ff_list <- FFdownload(
#   download = TRUE,
#   list     = "Fama/French 3 Factors",
#   freq     = "Daily",
#   tempd    = tempdir(),
#   destdir  = tempdir()
# )
# 
# # Handle package versions that name the container differently
# ff3_raw <- if ("FFdata" %in% names(ff_list)) ff_list$FFdata[[1]] else ff_list$data[[1]]
# 
# ff3 <- ff3_raw %>%
#   mutate(date = as.Date(date)) %>%
#   transmute(
#     date,
#     MKT_RF = `Mkt.RF` / 100,  # convert % → decimals
#     SMB    = SMB     / 100,
#     HML    = HML     / 100,
#     RF     = RF      / 100
#   )
# 
# # -------------------------
# # 3) Merge & build excess returns
# # -------------------------
# data_ff <- returns %>%
#   left_join(ff3, by = "date") %>%
#   mutate(
#     excess_ret = ret - RF   # Ri - Rf
#   ) %>%
#   drop_na()
# 
# # Panel data object
# pdata <- pdata.frame(data_ff, index = c("symbol","date"))
# 
# # -------------------------
# # 4) Regressions
# # -------------------------
# # CAPM: (Ri - Rf) ~ (Rm - Rf)
# capm <- plm(excess_ret ~ MKT_RF, data = pdata, model = "pooling")
# 
# # FF3: (Ri - Rf) ~ (Rm - Rf) + SMB + HML
# ff3_mod <- plm(excess_ret ~ MKT_RF + SMB + HML, data = pdata, model = "pooling")
# 
# cat("\n=== CAPM (pooling) ===\n")
# print(summary(capm))
# 
# cat("\n=== Fama–French 3 (pooling) ===\n")
# print(summary(ff3_mod))
# 
# # -------------------------
# # 5) Robust SEs (recommended for returns)
# #    - Cluster by firm (group) for panel-robust inference
# # -------------------------
# capm_rob  <- coeftest(capm,   vcov = vcovHC(capm,   type = "HC1", cluster = "group"))
# ff3_rob   <- coeftest(ff3_mod, vcov = vcovHC(ff3_mod, type = "HC1", cluster = "group"))
# 
# cat("\n=== CAPM with cluster-robust SEs (by firm) ===\n")
# print(capm_rob)
# 
# cat("\n=== FF3 with cluster-robust SEs (by firm) ===\n")
# print(ff3_rob)
# 
# 





