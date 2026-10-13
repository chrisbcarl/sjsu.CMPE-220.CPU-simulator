---
header-includes: |
title: >
  2026F - SJSU - CMPE 220 - project - chris_carl
subtitle:
author: Chris Carl <chris.carl@sjsu.edu>
date: 2026-09-22
toc: true
geometry:
    - "margin=1.5in"
    # - landscape
template: "math"  # "chicago" "math"
# ieee
abstract: This is my submission of project of CMPE 220 at SJSU instructed by Dr. Ishie Eswar in Fall 2026.
keywords: normal distribution, exponential distribution, random variables
# custom
doublespaced: false
course: SJSU 2026F - CMPE 220 - System Software
authors:
    - name: Chris Carl
      email: chris.carl@sjsu.edu
      institution: San Jose State University
      location: San Jose, CA, United States of America
      occupation: Masters of Science in Computer Engineering Student
---

<!--
Updates:
    2026-09-22 22:24 - project - initial commit

Examples:
    md2pdf "CMPE-220/assignments/project/project.md" `
        -o "CMPE-220/assignments/project/render" -ss -alc

    pandoc "CMPE-220/assignments/project/project.md" `
        --from=gfm --to=pdf --standalone --mathjax --toc-depth=4 `
        --resource-path "CMPE-220/assignments/project" `
        --output "CMPE-220/assignments/project/project.pdf"
-->


# Question 1

The purpose of this problem is to review using the $z$-table (which can be found on Canvas) to find probabilities involving standardized normal random variables. Let $Z$ be a standard normal random variable so that $Z \sim \mathcal{N}(0, 1)$. Recall, that $\Phi(z)$ denotes the standard normal CDF.

- (a) Find $\Phi(1.45)$

    $\Phi(1.45) \approx .9265$

- (b) Find a interval of the form (c, 1.45) so that $P[Z \in (c,1.45)] = 0.9$

    $$
    %\label{ans-1-b}
    \begin{aligned}
        0.9 &= P[Z \in (c,1.45)] && \text{given, equality} \\
        &= P[c \le Z \le 1.45] && \text{equivalent} \\
        &= P[Z \le 1.45] - P[Z \le c] && \text{equivalent} \\
        &\text{note: all terms normalized} \\
    0.9 &= \Phi(1.45) - \Phi(c) && \text{equivalent} \\
    0.9 &= 0.9265 - \Phi(c) && \text{evaluate z-table} \\
    \Phi(c) &= 0.9265 - 0.9 && \text{algebra} \\
    \Phi^{-1}(\Phi(c)) &= \Phi^{-1}(0.0265) && \text{distributive} \\
    c &\approx -1.935 && \text{} \\
    \end{aligned}
    $$

    $(-1.935, 1.45)$


- (c) Find $z_0$, such that $\Phi(z_0) = 0.04$.

    $$
    %\label{ans-1-c}
    \begin{aligned}
    \Phi(z_0) &= 0.04 &&\text{given} \\
    \Phi^{-1}(\Phi(z_0)) &= \Phi^{-1}(0.04) &&\text{distributive} \\
    z_0 &\approx -1.75 && \text{} \\
    \end{aligned}
    $$

    Verified with <p-to-z>.


# Appendix

# Notes
- [Nice Visual Z-Calculator](https://ztable.io/)
    - [Functional Z-Calculator, covers more ground](https://www.calculator.net/z-score-calculator.html)
- Computational Approximations of $\Phi$
    - [BETTER APPROXIMATIONS TO CUMULATIVE NORMAL FUNCTIONS - GRAEME WEST](https://s2.smu.edu/~aleskovs/emis/sqc2/accuratecumnorm.pdf)
    - [2009 - A logistic approximation to the cumulative normal distribution - Bowling1, Khasawneh2, Kaewkuekool3, Cho4](https://files01.core.ac.uk/download/pdf/41787448.pdf?__cf_chl_tk=MWsRZp6.6se3uxYbdAgEo6h9vM.WSE86ewioNnpmqmQ-1770269032-1.0.1.1-SOVYO0gbtebTAINUVZBkwVak4zyKfOzFSnrq4aiuGcA)
        - REALLY cool
    - unused in this assingment but fascinating to track the progression of discoveries.

## Reminder of Concepts
- $\Phi$ is the CDF of the standard normal distribution, so $\Phi(z)$ is the statement of "integrate from the left up to z".
    - $\Phi(z) = P(Z \le z) = \text{Z-table area from z to the left}$
    - Note: conceptually, this is the same as $F(x) = P(X \le x)$ for other distributions.
- pdf / pmf - probability density function (continuous) / probability density function (discrete)
    - function that maps random variable values to probabilities
    - $f(x) = \cdots$
- CDF - cumulative distribution function
    - function that maps random variable values to (what is the integration of probability?)
    - $F(x) = \int_{-\infty}^{x} f(t)dt$
    - $\Phi(z)$ - normal CDF



# Proofs

## Proof of Normal Distribution Even Symmetry About Constant Mu
$$
%\label{proof-mu-0}
\begin{aligned}
\text{let } &\mu = 0, x \ge \mu, r \in \mathbb{R} \\
\text{define } f(x) &: \mathbb{R} \longrightarrow \mathbb{R}(0, 1) = \frac{1}{\sqrt{2\pi\sigma^{2}}}e^{-{\frac{(x - \mu)^{2}}{2\sigma^{2}}}} && \text{normal distribution PDF} \\
f(x) &= f(-x), f(x) - f(-x) = 0 && \text{definition of even symmetry} \\
\\
f(-r) &= \frac{1}{\sqrt{2\pi\sigma^{2}}}e^{-{\frac{(-r-0)^{2}}{2\sigma^{2}}}} = \frac{1}{\sqrt{2\pi\sigma^{2}}}e^{-{\frac{r^{2}}{2\sigma^{2}}}} &&\text{pdf evaluated for lower bound, } \mu = 0 \\
f(r) &= \frac{1}{\sqrt{2\pi\sigma^{2}}}e^{-{\frac{(r-0)^{2}}{2\sigma^{2}}}} = \frac{1}{\sqrt{2\pi\sigma^{2}}}e^{-{\frac{r^{2}}{2\sigma^{2}}}} &&\text{pdf evaluated for lower bound, } \mu = 0 \\
f(-r) &= f(r) \\
&\rule{4cm}{0.1pt}\\
&\therefore QED\\
\end{aligned}
$$



<!--
# BIBLIOGRAPHY


- 0.04 probability from z = -1.75
https://www.calculator.net/z-score-calculator.html?c2z=&c2p=0.04&c2pg=&c2p0=&c2pin=&c2pout=&calctype=converter&x=Calculate#converter
```bibtex
@misc{p-to-z,
  author       = "calculator.net",
  title        = "Z-score and Probability Converter",
  url = "https://www.calculator.net/z-score-calculator.html?c2z=&c2p=0.04&c2pg=&c2p0=&c2pin=&c2pout=&calctype=converter&x=Calculate#converter",
  note         = "Accessed: 2026-02-02",
}
```


```bibtex
@misc{z-range,
  author       = "calculator.net",
  title        = "Z-score and Probability Converter",
  url = "https://www.calculator.net/z-score-calculator.html?c3z1=-1.705&c3z2=0.74&calctype=range&x=Calculate#range",
  note         = "Accessed: 2026-02-02",
}
```

```bibtex
@misc{range-95,
  author       = "calculator.net",
  title        = "Z-score and Probability Converter",
  url = "https://www.calculator.net/z-score-calculator.html?c3z1=-1.9&c3z2=2.03&calctype=range&x=Calculate#range",
  note         = "Accessed: 2026-02-02",
}
```
-->


