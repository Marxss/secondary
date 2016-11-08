#include "functions.h"

typedef struct {
  size_t num;
  double total;
} __agg_sumCtx;

int __agg_sumStep(AggCtx *ctx, SIValue *argv, int argc) {

  // convert the value of the input sequence to a double if possible
  double n;
  if (!SIValue_ToDouble(&argv[0], &n)) {
    if (!SIValue_IsNullPtr(&argv[0])) {
      // not convertible to double!
      return AGG_ERR;
    }
  }

  __agg_sumCtx *ac = Agg_FuncCtx(ctx);

  ac->num++;
  ac->total += n;
  return AGG_OK;
}

int __agg_sumFinalize(AggCtx *ctx) {
  Agg_SetState(ctx, AGG_STATE_DONE);
  return AGG_OK;
}

int __ag_avgFinalize(AggCtx *ctx) {
  __agg_sumCtx *ac = Agg_FuncCtx(ctx);
  if (ac->total > 0) {
    ac->total /= (double)ac->num;
  }

  Agg_SetState(ctx, AGG_STATE_DONE);
  return AGG_OK;
}

int __agg_sumReduceNext(AggCtx *ctx) {

  __agg_sumCtx *ac = Agg_FuncCtx(ctx);

  Agg_SetResult(ctx, SI_DoubleVal(ac->total));
  // this is
  Agg_EOF(ctx);

  return AGG_OK;
}

AggPipelineNode Agg_SumFunc(AggPipelineNode *in) {

  __agg_sumCtx *ac = malloc(sizeof(__agg_sumCtx));
  ac->num = 0;
  ac->total = 0;

  return Agg_Reduce(in, ac, __agg_sumStep, __agg_sumFinalize,
                    __agg_sumReduceNext, 1);
}

AggPipelineNode Agg_AverageFunc(AggPipelineNode *in) {
  __agg_sumCtx *ac = malloc(sizeof(__agg_sumCtx));
  ac->num = 0;
  ac->total = 0;

  return Agg_Reduce(in, ac, __agg_sumStep, __ag_avgFinalize,
                    __agg_sumReduceNext, 1);
}