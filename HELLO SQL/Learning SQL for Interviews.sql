# 📊 SQL for Quant Interview Prep — 20 Essential Queries

SQL is heavily tested in quant interviews for data handling, joins, and time-series prep.  
Here are 20 must-know queries (with finance-style examples).  

---

## 1. Basic Queries
```sql
-- Select specific columns
SELECT ticker, price FROM stocks;

-- Filter with conditions
SELECT * FROM trades WHERE volume > 100000;

-- Sorting results
SELECT * FROM prices ORDER BY date DESC;

-- Limit results
SELECT * FROM prices ORDER BY date DESC LIMIT 10;

-- Distinct values
SELECT DISTINCT ticker FROM trades;
```
## 2. Aggregate Functions
```sql
-- Count rows
SELECT COUNT(*) FROM trades WHERE ticker='AAPL';

-- Average / sum
SELECT ticker, AVG(price) AS avg_price 
FROM prices 
GROUP BY ticker;

-- Group by with multiple metrics
SELECT ticker, AVG(volume), MAX(price), MIN(price)
FROM trades
GROUP BY ticker;

-- Filter groups (HAVING)
SELECT ticker, AVG(price) 
FROM prices 
GROUP BY ticker
HAVING AVG(price) > 100;

-- Date-based grouping
SELECT DATE(time) AS trade_day, SUM(volume)
FROM trades
GROUP BY trade_day;
```

## 3. Joins
```sql
-- Inner Join
SELECT p.date, p.ticker, p.price, f.pe_ratio
FROM prices p
JOIN fundamentals f
  ON p.ticker = f.ticker;

-- Left Join
SELECT p.date, p.ticker, f.dividend_yield
FROM prices p
LEFT JOIN fundamentals f
  ON p.ticker = f.ticker;

-- Self Join (lag returns example)
SELECT a.ticker, a.date, a.price AS today_price, b.price AS yesterday_price
FROM prices a
JOIN prices b
  ON a.ticker = b.ticker AND a.date = b.date + 1;

-- Join multiple tables
SELECT t.ticker, t.date, t.volume, p.price
FROM trades t
JOIN prices p ON t.ticker = p.ticker AND t.date = p.date
JOIN fundamentals f ON t.ticker = f.ticker;
```

## 4. Window Functions 
```sql
-- Running total
SELECT date, ticker, 
       SUM(volume) OVER (PARTITION BY ticker ORDER BY date) AS cum_volume
FROM trades;

-- Moving average
SELECT date, ticker, 
       AVG(price) OVER (PARTITION BY ticker ORDER BY date ROWS BETWEEN 4 PRECEDING AND CURRENT ROW) AS ma5
FROM prices;

-- Ranking
SELECT ticker, date, price,
       RANK() OVER (PARTITION BY date ORDER BY price DESC) AS rank_on_day
FROM prices;

-- Lag/Lead (returns)
SELECT ticker, date, price,
       LAG(price, 1) OVER (PARTITION BY ticker ORDER BY date) AS prev_price
FROM prices;

-- Percentile rank
SELECT ticker, date, price,
       PERCENT_RANK() OVER (PARTITION BY date ORDER BY price) AS pct_rank
FROM prices;
```

## 5. Advanced Techniques
```sql
-- Subquery (max price for TSLA)
SELECT * FROM prices
WHERE price = (SELECT MAX(price) FROM prices WHERE ticker='TSLA');

-- CTE (Common Table Expression) for daily returns
WITH daily_returns AS (
  SELECT ticker, date,
         price / LAG(price) OVER (PARTITION BY ticker ORDER BY date) - 1 AS ret
  FROM prices
)
SELECT ticker, AVG(ret) AS avg_return
FROM daily_returns
GROUP BY ticker;

-- Pivot (monthly returns)
SELECT ticker,
       SUM(CASE WHEN MONTH(date) = 1 THEN ret ELSE 0 END) AS Jan,
       SUM(CASE WHEN MONTH(date) = 2 THEN ret ELSE 0 END) AS Feb,
       SUM(CASE WHEN MONTH(date) = 3 THEN ret ELSE 0 END) AS Mar
FROM daily_returns
GROUP BY ticker;    
```


# 📊 SQL for Quant Interview Prep — 50 Essential Queries

SQL is heavily tested in quant interviews for **data handling, joins, and time-series prep**.  
These 50 queries cover ~97.5% of what you’ll face.

---

## 1. Basic Queries
```sql
-- Select specific columns
SELECT ticker, price FROM stocks;

-- Filter with conditions
SELECT * FROM trades WHERE volume > 100000;

-- Sorting results
SELECT * FROM prices ORDER BY date DESC;

-- Limit results
SELECT * FROM prices ORDER BY date DESC LIMIT 10;

-- Distinct values
SELECT DISTINCT ticker FROM trades;
```
2. Aggregation & Grouping
```sql
-- Count rows
SELECT COUNT(*) FROM trades WHERE ticker='AAPL';

-- Average / sum
SELECT ticker, AVG(price) AS avg_price 
FROM prices 
GROUP BY ticker;

-- Group by with multiple metrics
SELECT ticker, AVG(volume), MAX(price), MIN(price)
FROM trades
GROUP BY ticker;

-- Filter groups (HAVING)
SELECT ticker, AVG(price) 
FROM prices 
GROUP BY ticker
HAVING AVG(price) > 100;

-- Date-based grouping
SELECT DATE(time) AS trade_day, SUM(volume)
FROM trades
GROUP BY trade_day;
```
3. Joins
```sql
-- Inner Join
SELECT p.date, p.ticker, p.price, f.pe_ratio
FROM prices p
JOIN fundamentals f
  ON p.ticker = f.ticker;

-- Left Join
SELECT p.date, p.ticker, f.dividend_yield
FROM prices p
LEFT JOIN fundamentals f
  ON p.ticker = f.ticker;

-- Self Join (lag returns example)
SELECT a.ticker, a.date, a.price AS today_price, b.price AS yesterday_price
FROM prices a
JOIN prices b
  ON a.ticker = b.ticker AND a.date = b.date + 1;

-- Join multiple tables
SELECT t.ticker, t.date, t.volume, p.price
FROM trades t
JOIN prices p ON t.ticker = p.ticker AND t.date = p.date
JOIN fundamentals f ON t.ticker = f.ticker;
```
4. Window Functions
```sql
-- Running total
SELECT date, ticker, 
       SUM(volume) OVER (PARTITION BY ticker ORDER BY date) AS cum_volume
FROM trades;

-- Moving average
SELECT date, ticker, 
       AVG(price) OVER (PARTITION BY ticker ORDER BY date ROWS BETWEEN 4 PRECEDING AND CURRENT ROW) AS ma5
FROM prices;

-- Ranking
SELECT ticker, date, price,
       RANK() OVER (PARTITION BY date ORDER BY price DESC) AS rank_on_day
FROM prices;

-- Lag/Lead (returns)
SELECT ticker, date, price,
       LAG(price, 1) OVER (PARTITION BY ticker ORDER BY date) AS prev_price
FROM prices;
```
5. Advanced Tricks
```sql
-- Subquery (max price for TSLA)
SELECT * FROM prices
WHERE price = (SELECT MAX(price) FROM prices WHERE ticker='TSLA');

-- CTE (Common Table Expression) for daily returns
WITH daily_returns AS (
  SELECT ticker, date,
         price / LAG(price) OVER (PARTITION BY ticker ORDER BY date) - 1 AS ret
  FROM prices
)
SELECT ticker, AVG(ret) AS avg_return
FROM daily_returns
GROUP BY ticker;
```

Advanced 30 Queries for 97.5% Coverage
A) Time-Series & Window Power-Ups
```sql
-- Median
SELECT ticker, percentile_cont(0.5) WITHIN GROUP (ORDER BY price) AS median_price
FROM prices GROUP BY ticker;

-- Deciles
SELECT date, ticker, ret,
       ntile(10) OVER (PARTITION BY date ORDER BY ret) AS decile
FROM daily_returns;

-- Rolling volatility
SELECT date, ticker,
       stddev_samp(ret) OVER (PARTITION BY ticker ORDER BY date ROWS BETWEEN 20 PRECEDING AND CURRENT ROW) AS vol_21
FROM daily_returns;

-- Exponential moving average (recursive CTE)
WITH RECURSIVE ema AS (
  SELECT date, ticker, price::float AS ema_val
  FROM prices
  WHERE (ticker, date) IN (
    SELECT ticker, MIN(date) FROM prices GROUP BY ticker
  )
  UNION ALL
  SELECT p.date, p.ticker,
         0.1*p.price + 0.9*e.ema_val
  FROM ema e
  JOIN prices p
    ON p.ticker = e.ticker AND p.date = e.date + INTERVAL '1 day'
)
SELECT * FROM ema;

-- Cumulative distinct count
SELECT date, ticker,
       COUNT(DISTINCT trader_id) OVER (PARTITION BY ticker ORDER BY date RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS cum_traders
FROM trades;

-- Top 3 per group
SELECT *
FROM (
  SELECT date, ticker, ret,
         dense_rank() OVER (PARTITION BY date ORDER BY ret DESC) AS rnk
  FROM daily_returns
) t
WHERE rnk <= 3;

-- Sessionization (30 min gap = new session)
SELECT trader_id, time, 
       SUM(CASE WHEN lag(time) OVER (PARTITION BY trader_id ORDER BY time) < time - INTERVAL '30 min' 
                OR lag(time) IS NULL THEN 1 ELSE 0 END)
         OVER (PARTITION BY trader_id ORDER BY time) AS session_id
FROM clicks;

-- Detect gaps in trading days
WITH tagged AS (
  SELECT ticker, date,
         ROW_NUMBER() OVER (PARTITION BY ticker ORDER BY date) AS rn
  FROM prices
),
groups AS (
  SELECT ticker, date, date - rn * INTERVAL '1 day' AS grp
  FROM tagged
)
SELECT ticker, MIN(date) AS start_missing_next_day, MAX(date) AS end_consecutive
FROM groups
GROUP BY ticker, grp;

-- As-of join (last known price)
SELECT t.trade_id, t.ticker, t.time, p.price
FROM trades t
JOIN LATERAL (
  SELECT price
  FROM prices p
  WHERE p.ticker = t.ticker AND p.time <= t.time
  ORDER BY p.time DESC
  LIMIT 1
) p ON true;

-- Forward fill last non-null
SELECT date, ticker,
       COALESCE(val,
         LAST_VALUE(val) OVER (PARTITION BY ticker ORDER BY date RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW)
       ) AS ffilled
FROM series;
```
B) Joins, Set Ops, & Reshaping
```sql
-- Semi-join
SELECT * FROM prices p
WHERE EXISTS (SELECT 1 FROM universe u WHERE u.ticker = p.ticker);

-- Anti-join
SELECT p.* FROM prices p
WHERE NOT EXISTS (SELECT 1 FROM blacklist b WHERE b.ticker = p.ticker);

-- UNION
SELECT ticker FROM list_a
UNION
SELECT ticker FROM list_b;

-- INTERSECT / EXCEPT
SELECT ticker FROM list_a INTERSECT SELECT ticker FROM list_b;

-- Pivot (wide format)
SELECT * FROM crosstab(
  $$SELECT date, ticker, ret FROM daily_returns ORDER BY 1,2$$
) AS ct(date date, AAPL float, MSFT float, NVDA float);

-- Unpivot (long format)
SELECT date, key AS ticker, val AS ret
FROM wide_returns
CROSS JOIN LATERAL json_each_text(to_json(wide_returns)) AS j(key, val)
WHERE key <> 'date';
```

C) Data Quality & Cleaning
```sql
-- Deduplicate
SELECT * FROM (
  SELECT *, ROW_NUMBER() OVER (PARTITION BY ticker, date ORDER BY updated_at DESC) AS rn
  FROM prices_raw
) s WHERE rn = 1;

-- Conditional aggregation
SELECT date,
       SUM(CASE WHEN ticker='AAPL' THEN 1 ELSE 0 END) AS aapl_rows,
       SUM(CASE WHEN ticker='MSFT' THEN 1 ELSE 0 END) AS msft_rows
FROM prices GROUP BY date;

-- Winsorize
WITH b AS (
  SELECT date,
         percentile_cont(0.01) WITHIN GROUP (ORDER BY ret) AS p1,
         percentile_cont(0.99) WITHIN GROUP (ORDER BY ret) AS p99
  FROM daily_returns GROUP BY date
)
SELECT r.date, r.ticker, LEAST(GREATEST(r.ret, b.p1), b.p99) AS ret_wins
FROM daily_returns r JOIN b USING (date);

-- Z-score standardization
SELECT date, ticker,
       (ret - AVG(ret) OVER (PARTITION BY date))
       / NULLIF(stddev_samp(ret) OVER (PARTITION BY date),0) AS z_ret
FROM daily_returns;

-- Adjust for splits
SELECT p.date, p.ticker, p.price / s.factor AS adj_price
FROM prices p
LEFT JOIN splits s ON s.ticker = p.ticker AND s.date <= p.date;

-- Business-day difference
SELECT a.ticker, a.date, b.date,
       COUNT(*) AS biz_days_between
FROM cal c
JOIN prices a ON c.date > a.date
JOIN prices b ON b.ticker = a.ticker AND c.date <= b.date
WHERE c.is_business_day
GROUP BY a.ticker, a.date, b.date;
```
D) Analytics & Factor Construction
```sql
-- Correlation
SELECT date, corr(x, y) AS corr_xy
FROM factor_matrix GROUP BY date;

-- Covariance
SELECT date, f1, f2, covar_samp(val1, val2) AS cov
FROM factor_pairs GROUP BY date, f1, f2;

-- Beta vs market
SELECT ticker, regr_slope(ret, mkt_ret) AS beta
FROM joined_returns GROUP BY ticker;

-- Portfolio turnover
WITH w AS (
  SELECT date, ticker, weight,
         LAG(weight) OVER (PARTITION BY ticker ORDER BY date) AS prev_w
  FROM port_weights
)
SELECT date, SUM(ABS(weight - COALESCE(prev_w,0))) AS turnover
FROM w GROUP BY date;

-- Month-end snapshot
SELECT DISTINCT ON (ticker, date_trunc('month', date))
       date_trunc('month', date) AS ym, ticker, 
       FIRST_VALUE(price) OVER (PARTITION BY ticker, date_trunc('month', date) ORDER BY date DESC) AS month_end_price
FROM prices;

-- VWAP
SELECT date, ticker,
       SUM(price*volume)::float / NULLIF(SUM(volume),0) AS vwap
FROM trades GROUP BY date, ticker;

-- Neutralization
WITH x AS (
  SELECT date, ticker, ret, ln(mktcap) AS ln_size
  FROM cross_section
),
fit AS (
  SELECT date, regr_slope(ret, ln_size) AS b1, avg(ret) AS a
  FROM x GROUP BY date
)
SELECT x.date, x.ticker,
       x.ret - (f.a + f.b1*x.ln_size) AS ret_neutral
FROM x JOIN fit f USING (date);

-- Decile long-short portfolio
WITH ranked AS (
  SELECT date, ticker, z_score,
         ntile(10) OVER (PARTITION BY date ORDER BY z_score) AS decile
  FROM z_table
),
weights AS (
  SELECT date, ticker,
         CASE WHEN decile=10 THEN 1.0
              WHEN decile=1  THEN -1.0
              ELSE 0 END AS w_raw
  FROM ranked
),
norm AS (
  SELECT date,
         SUM(CASE WHEN w_raw>0 THEN w_raw END) AS long_sum,
         SUM(CASE WHEN w_raw<0 THEN -w_raw END) AS short_sum
  FROM weights GROUP BY date
)
SELECT w.date, w.ticker,
       CASE WHEN w_raw>0 THEN w_raw/NULLIF(n.long_sum,0)
            WHEN w_raw<0 THEN -w_raw/NULLIF(n.short_sum,0)
            ELSE 0 END AS weight_norm
FROM weights w JOIN norm n USING (date);
```
E) JSON / Arrays / LATERAL Tricks
```sql
-- Parse JSON
SELECT id, (payload->>'symbol') AS symbol, (payload->>'lastPrice')::float AS last_price
FROM raw_json_ticks;

-- Explode arrays
SELECT f.ticker, x.key AS factor, (x.value)::float AS exposure
FROM factor_json f
CROSS JOIN LATERAL json_each(f.exposures) AS x(key, value);

-- CROSS APPLY / LATERAL
SELECT t.order_id, s.*
FROM orders t
CROSS JOIN LATERAL (
  SELECT SUM(filled_qty) AS filled, SUM(filled_qty*price)/NULLIF(SUM(filled_qty),0) AS avg_fill
  FROM fills f WHERE f.order_id = t.order_id
) s;
F) Engineering & Generation Utilities
sql
Copy code
-- Generate series (calendar scaffold)
SELECT d::date AS dt
FROM generate_series('2024-01-01'::date, '2025-12-31'::date, INTERVAL '1 day') d;

-- Recursive hierarchy
WITH RECURSIVE tree AS (
  SELECT id, parent_id, 0 AS lvl FROM nodes WHERE parent_id IS NULL
  UNION ALL
  SELECT n.id, n.parent_id, t.lvl+1
  FROM nodes n JOIN tree t ON n.parent_id = t.id
)
SELECT * FROM tree;

