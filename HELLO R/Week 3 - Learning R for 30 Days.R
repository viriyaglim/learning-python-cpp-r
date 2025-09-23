#Hello R
print("Hello Viriya from R in VS Code!")
#Week 3 - This is 30 days learning R

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

# 🔹 Week 3: Time Series & Volatility Modeling
# 
# Goal: Work with ARIMA, VAR, ARCH/GARCH.
# 
# Day 15: Time series basics.
# Stationarity, ACF/PACF plots.
# 
# Package: tseries.

# ---- Get AAPL daily log returns robustly (uses tq_transmute)
aapl_ret <- tq_get("AAPL", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  tq_transmute(
    select     = adjusted,
    mutate_fun = periodReturn,
    period     = "daily",
    type       = "log",
    col_rename = "ret"     # name the return column directly
  ) %>%
  tidyr::drop_na()          # remove the first NA

# Quick sanity checks (optional)
stopifnot(nrow(aapl_ret) > 0)
# print(summary(aapl_ret$ret)); print(head(aapl_ret))

# ---- Make sure we have a plain numeric vector
returns <- aapl_ret %>% dplyr::pull(ret) %>% as.numeric()

# ---- ADF stationarity test
adf_test <- adf.test(returns)
print(adf_test)

# ---- Time-series plot
ggplot(aapl_ret, aes(x = date, y = ret)) +
  geom_line(color = "steelblue") +
  labs(title = "AAPL Daily Log Returns (2023)",
       x = "Date", y = "Log Return")

# ---- ACF / PACF (allow any stray NA just in case)
acf(returns, na.action = na.pass, main = "ACF of AAPL Returns")
pacf(returns, na.action = na.pass, main = "PACF of AAPL Returns")

# ACF (Autocorrelation Function): shows correlation of the series with lags.
# 
# PACF (Partial ACF): shows direct correlation at each lag after controlling for intermediates.
# 
# 👉 Interpretation:
#   
# If ACF tails off slowly → non-stationary (common in prices).
# 
# If ACF/PACF cut off quickly → stationary (common in returns).

###

# 1. ACF of AAPL Returns
#This means there is no significant autocorrelation in AAPL daily log returns.
# 
# In finance, this is expected: returns tend to be serially uncorrelated (weak-form efficiency).
# 
# 2. PACF of AAPL Returns
# 
# PACF also shows small spikes, mostly within the blue bands.
# 
# No strong evidence of partial autocorrelation at specific lags.
# 
# This reinforces the idea that returns are essentially white noise (unpredictable from past returns).
# AAPL daily returns are stationary (after log-differencing).
# 
# No significant autocorrelation patterns → a simple ARIMA(0,0,0) (white noise) could model the returns themselves.
#However, in financial data, while returns are uncorrelated, squared/absolute returns often show autocorrelation (volatility clustering). That’s what motivates ARCH/GARCH models, which we’ll see later.




#----------------------------

# Day 16: ARIMA modeling.
# 
# Package: forecast.
# 
# Task: Forecast AAPL daily returns.

## 1) Get AAPL daily log returns as a 1-D numeric vector
aapl_ret <- tq_get("AAPL", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  tq_transmute(select = adjusted,
               mutate_fun = periodReturn,
               period = "daily",
               type = "log",
               col_rename = "ret") %>%
  drop_na()

ret_vec <- as.numeric(aapl_ret$ret)      # <- single vector
ret_ts  <- ts(ret_vec, frequency = 252)  # 252 trading days/year

## 2) Fit ARIMA (turn off fast approximation to be safe)
fit <- auto.arima(ret_ts, seasonal = FALSE,
                  stepwise = FALSE, approximation = FALSE)

summary(fit)              # model chosen, coefficients, Ljung–Box p-value
checkresiduals(fit)       # residual ACF + LB test

## 3) Forecast next 10 trading days
fc <- forecast(fit, h = 10)
print(fc)
autoplot(fc) +
  labs(title = "ARIMA Forecast of AAPL Daily Returns",
       x = "Time", y = "Return")
# 
# Day 17: Vector AutoRegression (VAR).
# 
# Package: vars.
# 
# Task: Model interaction between stock index & bond yields.

## 1) Download data (unique object names)
spx_tbl <- tq_get("^GSPC", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  transmute(date, spx = adjusted)

y10_tbl <- tq_get("DGS10", get = "economic.data",
                  from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  transmute(date, y10 = price)   # percent level, e.g., 4.25

## 2) Merge and build stationary series
var_df <- spx_tbl %>%
  inner_join(y10_tbl, by = "date") %>%
  mutate(
    mkt_ret = log(spx / dplyr::lag(spx)),      # stock log return
    y10_chg = (y10 - dplyr::lag(y10)) / 100    # yield change in decimals
  ) %>%
  drop_na() %>%
  select(date, mkt_ret, y10_chg)

# quick checks
stopifnot(nrow(var_df) > 10, all(is.finite(var_df$mkt_ret)), all(is.finite(var_df$y10_chg)))

## 3) Stationarity (ADF)
print(adf.test(var_df$mkt_ret))
print(adf.test(var_df$y10_chg))

## 4) Build VAR input (matrix with clear colnames)
var_mat <- as.matrix(var_df[, c("mkt_ret","y10_chg")])
colnames(var_mat) <- c("mkt_ret","y10_chg")

## 5) Choose lag order and fit VAR
lag_info <- VARselect(var_mat, lag.max = 10, type = "const")
print(lag_info$selection)

p_opt <- as.numeric(lag_info$selection["AIC(n)"])
if (is.na(p_opt)) p_opt <- 2  # sensible fallback

var_fit <- VAR(var_mat, p = p_opt, type = "const")
summary(var_fit)

## 6) Diagnostics
# Stability: all roots' moduli < 1
print(roots(var_fit, modulus = TRUE))

# Serial correlation (Portmanteau)
print(serial.test(var_fit, lags.pt = 12, type = "PT.asymptotic"))

# Heteroskedasticity (ARCH LM)
print(arch.test(var_fit, lags.multi = 12))

## 7) Granger causality
# Does market return help predict yield changes?
print(causality(var_fit, cause = "mkt_ret")$Granger)
# Do yield changes help predict market returns?
print(causality(var_fit, cause = "y10_chg")$Granger)

## 8) Impulse responses (10 steps, bootstrapped bands)
irf_mkt  <- irf(var_fit, impulse = "mkt_ret",  response = c("mkt_ret","y10_chg"),
                n.ahead = 10, boot = TRUE, runs = 1000)
irf_y10  <- irf(var_fit, impulse = "y10_chg", response = c("mkt_ret","y10_chg"),
                n.ahead = 10, boot = TRUE, runs = 1000)

plot(irf_mkt)   # shock to stocks
plot(irf_y10)   # shock to yields

## 9) 10-step ahead forecast (both series)
var_fc <- predict(var_fit, n.ahead = 10)
plot(var_fc)

#......



# ----------------------------------- 
# Day 18: ARCH model.
# 
# Package: tseries.
# 
# Task: Fit ARCH(1) to BTC returns.


## 1) Prices → daily log returns (demeaned)
btc_px_tbl <- tq_get("BTC-USD", from = "2024-01-01", to = "2024-12-31") %>%
  arrange(date) %>%
  transmute(date, px = adjusted)

btc_ret_tbl <- btc_px_tbl %>%
  mutate(ret = log(px / dplyr::lag(px))) %>%
  drop_na()

btc_ret_vec <- as.numeric(btc_ret_tbl$ret)
btc_ret_vec <- btc_ret_vec - mean(btc_ret_vec, na.rm = TRUE)  # mean ≈ 0

stopifnot(length(btc_ret_vec) > 50, all(is.finite(btc_ret_vec)))

## 2) Fit ARCH(1): order = c(ARCH, GARCH) = c(1,0)
arch_fit <- garch(x = btc_ret_vec, order = c(1, 0), trace = FALSE)
print(summary(arch_fit))  # shows a0 (omega) and a1 (ARCH coef)

## 3) Extract standardized residuals
arch_std_resid <- residuals(arch_fit)  # standardized residuals (variance ≈ 1)

## 4) Build in-sample conditional volatility σ_t
# For ARCH(1):  σ_t^2 = ω + α * ε_{t-1}^2  (with ε_t ≈ return_t here, mean ~ 0)
coefs <- coef(arch_fit)
omega <- unname(coefs["a0"])  # ω > 0
alpha <- unname(coefs["a1"])  # 0 < α < 1 for a valid ARCH(1)

n <- length(btc_ret_vec)
sigma2 <- numeric(n)
# initialize with unconditional variance or sample variance
sigma2[1] <- var(btc_ret_vec, na.rm = TRUE)
for (t in 2:n) {
  sigma2[t] <- omega + alpha * (btc_ret_vec[t - 1]^2)
}
sigma_t <- sqrt(sigma2)

## 5) Combine for plotting
plot_tbl <- btc_ret_tbl %>%
  mutate(
    std_resid = arch_std_resid,
    cond_sigma = sigma_t
  )

## 6) Plots
# a) Returns vs conditional volatility (σ_t)
ggplot(plot_tbl, aes(x = date)) +
  geom_line(aes(y = ret), linewidth = 0.4) +
  geom_line(aes(y = cond_sigma), linewidth = 0.7, linetype = "dashed") +
  labs(title = "BTC Daily Log Returns & ARCH(1) Conditional Volatility",
       x = "Date", y = "Value") +
  theme_minimal()

# b) Check volatility clustering captured: ACF of squared standardized residuals
acf(arch_std_resid^2, main = "ACF of Squared Standardized Residuals (ARCH(1))")

## 7) Quick console summary
cat("\n--- ARCH(1) parameters ---\n")
cat("omega (a0):", round(omega, 6), "\n")
cat("alpha (a1):", round(alpha, 6), "\n")
if (!is.na(alpha) && alpha > 0 && alpha < 1) {
  cat("=> Valid ARCH(1): alpha in (0,1) so variance is finite.\n")
} else {
  cat("=> Warning: alpha not in (0,1); model may be invalid.\n")
}

#HMMMMMM 



# ----------------------------
# Day 19: GARCH models.
# 
# Package: rugarch.
# 
# Task: Estimate GARCH(1,1) volatility for S&P500.
## 1) Download S&P500 daily data
spx_tbl <- tq_get("^GSPC", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  transmute(date, px = adjusted)

## 2) Log returns (demeaned)
spx_ret_tbl <- spx_tbl %>%
  mutate(ret = log(px / dplyr::lag(px))) %>%
  drop_na()

spx_ret_vec <- as.numeric(spx_ret_tbl$ret)
spx_ret_vec <- spx_ret_vec - mean(spx_ret_vec, na.rm = TRUE)

## 3) Specify GARCH(1,1) model
garch_spec <- ugarchspec(
  variance.model = list(model = "sGARCH", garchOrder = c(1, 1)),
  mean.model     = list(armaOrder = c(0, 0), include.mean = TRUE),
  distribution.model = "norm"   # Normal innovations
)

## 4) Fit model
garch_fit <- ugarchfit(spec = garch_spec, data = spx_ret_vec)
print(garch_fit)

## 5) Extract conditional volatility
cond_vol <- sigma(garch_fit)  # σ_t
plot_tbl <- spx_ret_tbl %>%
  mutate(cond_vol = cond_vol)

## 6) Plot returns vs volatility
ggplot(plot_tbl, aes(x = date)) +
  geom_line(aes(y = ret), color = "steelblue", linewidth = 0.4) +
  geom_line(aes(y = cond_vol), color = "darkred", linewidth = 0.6, linetype = "dashed") +
  labs(title = "S&P500 Daily Returns and GARCH(1,1) Conditional Volatility",
       x = "Date", y = "Value") +
  theme_minimal()

## 7) Diagnostic checks
# a) Standardized residuals
std_resid <- residuals(garch_fit, standardize = TRUE)

# b) Ljung-Box test on residuals & squared residuals
print(Box.test(std_resid, lag = 10, type = "Ljung-Box"))
print(Box.test(std_resid^2, lag = 10, type = "Ljung-Box"))

# c) ACF of squared residuals
acf(std_resid^2, main = "ACF of Squared Standardized Residuals (GARCH(1,1))")


# ---------------------------- 

# Day 20: EGARCH, GJR-GARCH, multivariate GARCH.
# 
# Package: rmgarch.
# ---------- 1) Data: S&P 500 & NASDAQ (daily) ----------
spx_px_tbl <- tq_get("^GSPC", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>% transmute(date, spx_px = adjusted)

ndx_px_tbl <- tq_get("^IXIC", from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>% transmute(date, ndx_px = adjusted)

ret_tbl <- inner_join(spx_px_tbl, ndx_px_tbl, by = "date") %>%
  mutate(
    spx_ret = log(spx_px / dplyr::lag(spx_px)),
    ndx_ret = log(ndx_px / dplyr::lag(ndx_px))
  ) %>%
  tidyr::drop_na()

spx_ret_vec <- as.numeric(ret_tbl$spx_ret)
ndx_ret_vec <- as.numeric(ret_tbl$ndx_ret)

# --------------------------------------------------------
# 2) EGARCH(1,1) on S&P500 (Student-t innovations)
# --------------------------------------------------------
egarch_spec <- ugarchspec(
  variance.model     = list(model = "eGARCH", garchOrder = c(1, 1)),
  mean.model         = list(armaOrder = c(0, 0), include.mean = TRUE),
  distribution.model = "std"
)

egarch_fit <- ugarchfit(
  spec = egarch_spec, data = spx_ret_vec,
  solver = "hybrid", fit.control = list(scale = 1)
)

show(egarch_fit)             # safe print
egarch_sigma <- sigma(egarch_fit)  # conditional volatility

# --------------------------------------------------------
# 3) GJR-GARCH(1,1) on S&P500 (robust settings)
#    (avoid Hessian inversion issues by not printing full
#     report; extract coefficients and sigma directly)
# --------------------------------------------------------
gjr_spec <- ugarchspec(
  variance.model     = list(model = "gjrGARCH", garchOrder = c(1, 1)),
  mean.model         = list(armaOrder = c(0, 0), include.mean = TRUE),
  distribution.model = "std"
)

gjr_fit <- ugarchfit(
  spec = gjr_spec, data = spx_ret_vec,
  solver = "hybrid",
  fit.control = list(scale = 1, fixed.se = 0, rec.init = "all")
)

# Don’t call show(gjr_fit) (can trigger Hessian warnings)
gjr_coef  <- coef(gjr_fit)
gjr_ic    <- infocriteria(gjr_fit)
gjr_sigma <- tryCatch(sigma(gjr_fit), error = function(e) rep(NA_real_, length(spx_ret_vec)))

cat("\nGJR-GARCH coefficients:\n"); print(gjr_coef)
cat("Info criteria (AIC,BIC,HQ):\n"); print(gjr_ic)

# --------------------------------------------------------
# 4) DCC-GARCH(1,1): dynamic correlation SPX ↔ NDX
# --------------------------------------------------------
uni_spec <- ugarchspec(
  variance.model     = list(model = "sGARCH", garchOrder = c(1, 1)),
  mean.model         = list(armaOrder = c(0, 0), include.mean = TRUE),
  distribution.model = "norm"
)

dcc_spec <- dccspec(
  uspec = multispec(list(uni_spec, uni_spec)),  # 2 series
  dccOrder = c(1, 1),
  distribution = "mvnorm"
)

dcc_fit <- dccfit(
  spec = dcc_spec,
  data = cbind(spx_ret_vec, ndx_ret_vec),
  fit.control = list(scale = 1)
)

# Dynamic correlation time series (SPX,NDX)
dcc_R <- rcor(dcc_fit)               # array: 2 x 2 x T
dcc_spx_ndx <- as.numeric(dcc_R[1, 2, ])

dcc_tbl <- ret_tbl %>% mutate(dcc_corr = dcc_spx_ndx)

# --------------------------------------------------------
# 5) Plots
# --------------------------------------------------------
# a) S&P500 returns vs EGARCH volatility
plot_egarch_tbl <- ret_tbl %>% mutate(egarch_vol = egarch_sigma)

ggplot(plot_egarch_tbl, aes(x = date)) +
  geom_line(aes(y = spx_ret), color = "steelblue", linewidth = 0.4) +
  geom_line(aes(y = egarch_vol), color = "darkred", linewidth = 0.6, linetype = "dashed") +
  labs(title = "S&P 500: Returns (blue) vs EGARCH(1,1) Volatility (red, dashed)",
       x = "Date", y = "Value") +
  theme_minimal()

# b) Dynamic correlation (DCC) SPX vs NDX
ggplot(dcc_tbl, aes(x = date, y = dcc_corr)) +
  geom_line(color = "darkgreen") +
  labs(title = "Dynamic Conditional Correlation: S&P 500 vs NASDAQ (DCC-GARCH)",
       x = "Date", y = "Correlation") +
  theme_minimal()
# ------------------------


# Day 21: Mini-project: Compare volatility models (GARCH vs EGARCH) for crypto.

# ---------- 1) Get BTC prices ----------
btc_tbl <- tq_get("BTC-USD",
                  from = "2023-01-01", to = "2023-12-31") %>%
  arrange(date) %>%
  transmute(date, btc_px = adjusted)

# ---------- 2) Compute log returns ----------
btc_ret_tbl <- btc_tbl %>%
  mutate(btc_ret = log(btc_px / dplyr::lag(btc_px))) %>%
  tidyr::drop_na()

btc_ret_vec <- as.numeric(btc_ret_tbl$btc_ret)

# ---------- 3) Fit GARCH(1,1) ----------
garch_spec <- ugarchspec(
  variance.model     = list(model = "sGARCH", garchOrder = c(1,1)),
  mean.model         = list(armaOrder = c(0,0), include.mean = TRUE),
  distribution.model = "std"
)

garch_fit <- ugarchfit(
  spec = garch_spec, data = btc_ret_vec,
  solver = "hybrid", fit.control = list(scale = 1)
)

# ---------- 4) Fit EGARCH(1,1) ----------
egarch_spec <- ugarchspec(
  variance.model     = list(model = "eGARCH", garchOrder = c(1,1)),
  mean.model         = list(armaOrder = c(0,0), include.mean = TRUE),
  distribution.model = "std"
)

egarch_fit <- ugarchfit(
  spec = egarch_spec, data = btc_ret_vec,
  solver = "hybrid", fit.control = list(scale = 1)
)

# ---------- 5) Extract conditional volatility ----------
btc_vol_tbl <- btc_ret_tbl %>%
  mutate(
    vol_garch  = sigma(garch_fit),
    vol_egarch = sigma(egarch_fit)
  )

# ---------- 6) Compare AIC, BIC ----------
garch_ic  <- infocriteria(garch_fit)
egarch_ic <- infocriteria(egarch_fit)

cat("\n--- Model Comparison ---\n")
print(rbind(GARCH = garch_ic, EGARCH = egarch_ic))

# ---------- 7) Plot volatility ----------
btc_vol_tbl_long <- btc_vol_tbl %>%
  tidyr::pivot_longer(cols = c(vol_garch, vol_egarch),
                      names_to = "model", values_to = "volatility")

ggplot(btc_vol_tbl_long, aes(x = date, y = volatility, color = model)) +
  geom_line() +
  labs(title = "BTC-USD: Conditional Volatility (GARCH vs EGARCH)",
       x = "Date", y = "Volatility") +
  theme_minimal()

